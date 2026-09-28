#!/usr/bin/env python3
"""Serves the recorded videos to a phone on the device's Wi-Fi hotspot, with a thumbnail player page.

Runs while the ShareVideos param is set. It clears the param and exits when the car starts moving,
or when nothing used it for IDLE_TIMEOUT. The player keeps it alive while a video plays.
"""
import asyncio
import ipaddress
import json
import os
import shutil
import time
from collections import OrderedDict

from aiohttp import web

import cereal.messaging as messaging
from openpilot.common.params import Params
from openpilot.common.swaglog import cloudlog
from openpilot.system.hardware.hw import Paths
from openpilot.system.mediaserver.media import CAMERA_FILES, SEGMENT_RE, get_thumbnail, list_routes, remux_to_mp4

from openpilot.system.mediaserver.constants import PORT
IDLE_TIMEOUT = 10 * 60  # seconds without requests
MOVING_SPEED = 1.0  # m/s
CHECK_INTERVAL = 1.0  # seconds
SHUTDOWN_TIMEOUT = 0.5  # seconds, open transfers are cut off instead of finishing
MAX_CACHED_VIDEOS = 6  # remuxed MP4s kept on disk, high resolution ones are up to ~75 MB
MAX_CACHED_THUMBNAILS = 500
# the hotspot (see TETHERING_IP_ADDRESS in wifi_manager.py) and this device
ALLOWED_NETWORKS = [ipaddress.ip_network(n) for n in ("192.168.43.0/24", "127.0.0.0/8", "::1/128")]
PLAYER_HTML = os.path.join(os.path.dirname(os.path.abspath(__file__)), "player.html")


def is_allowed(remote: str | None) -> bool:
  try:
    return any(ipaddress.ip_address(remote or "") in n for n in ALLOWED_NETWORKS)
  except ValueError:
    return False


def default_cache_dir() -> str:
  return os.path.join(os.path.dirname(os.path.normpath(Paths.log_root())), "mediaserver_cache")


class MediaServer:
  def __init__(self, log_root: str, cache_dir: str):
    self.log_root = log_root
    self.cache_dir = cache_dir
    self.last_activity = time.monotonic()
    self.thumbnails: OrderedDict[str, bytes | None] = OrderedDict()
    self.remux_locks: dict[str, asyncio.Lock] = {}

  def touch(self) -> None:
    self.last_activity = time.monotonic()

  def idle_time(self) -> float:
    return time.monotonic() - self.last_activity

  def segment_dir(self, segment: str) -> str:
    """Directory of a finished segment, rejecting anything else"""
    if SEGMENT_RE.match(segment) is None or "/" in segment or segment.startswith("."):
      raise web.HTTPNotFound()
    path = os.path.join(self.log_root, segment)
    if not os.path.isdir(path) or any(f.endswith(".lock") for f in os.listdir(path)):
      raise web.HTTPNotFound()
    return path

  def camera_file(self, segment: str, camera: str) -> str:
    if camera not in CAMERA_FILES:
      raise web.HTTPNotFound()
    path = os.path.join(self.segment_dir(segment), CAMERA_FILES[camera])
    if not os.path.isfile(path):
      raise web.HTTPNotFound()
    return path

  async def mp4(self, segment: str, camera: str) -> str:
    src = self.camera_file(segment, camera)
    dst = os.path.join(self.cache_dir, f"{segment}_{camera}.mp4")
    lock = self.remux_locks.setdefault(dst, asyncio.Lock())
    async with lock:
      if not os.path.isfile(dst):
        os.makedirs(self.cache_dir, exist_ok=True)
        await asyncio.to_thread(remux_to_mp4, src, dst)
        self.prune_cache(keep=dst)
    return dst

  def prune_cache(self, keep: str) -> None:
    files = sorted((os.path.join(self.cache_dir, f) for f in os.listdir(self.cache_dir) if f.endswith(".mp4")), key=os.path.getmtime)
    for path in files[:-MAX_CACHED_VIDEOS]:
      if path != keep:
        os.remove(path)

  async def thumbnail(self, segment: str) -> bytes | None:
    if segment not in self.thumbnails:
      self.thumbnails[segment] = await asyncio.to_thread(get_thumbnail, self.segment_dir(segment))
      while len(self.thumbnails) > MAX_CACHED_THUMBNAILS:
        self.thumbnails.popitem(last=False)
    return self.thumbnails[segment]

  # *** handlers ***

  async def index(self, request: web.Request) -> web.StreamResponse:
    return web.FileResponse(PLAYER_HTML, headers={"Cache-Control": "no-cache"})

  async def routes(self, request: web.Request) -> web.StreamResponse:
    routes = await asyncio.to_thread(list_routes, self.log_root)
    data = [{"name": r.name, "start": r.start_time,
             "segments": [{"name": s.name, "num": s.num, "cameras": s.cameras} for s in r.segments]} for r in routes]
    return web.Response(text=json.dumps(data), content_type="application/json")

  async def thumb(self, request: web.Request) -> web.StreamResponse:
    jpeg = await self.thumbnail(request.match_info["segment"])
    if jpeg is None:
      raise web.HTTPNotFound()
    return web.Response(body=jpeg, content_type="image/jpeg", headers={"Cache-Control": "private, max-age=86400"})

  async def video(self, request: web.Request) -> web.StreamResponse:
    segment, camera = request.match_info["segment"], request.match_info["camera"]
    path = await self.mp4(segment, camera)
    headers = {"Cache-Control": "private"}
    if "dl" in request.query:
      headers["Content-Disposition"] = f'attachment; filename="{segment}_{camera}.mp4"'
    return web.FileResponse(path, headers=headers)

  async def alive(self, request: web.Request) -> web.StreamResponse:
    return web.Response(status=204)

  @web.middleware
  async def access(self, request: web.Request, handler):
    if not is_allowed(request.remote):
      raise web.HTTPForbidden()
    self.touch()
    return await handler(request)

  def make_app(self) -> web.Application:
    app = web.Application(middlewares=[self.access])
    app.router.add_get("/", self.index)
    app.router.add_get("/api/routes", self.routes)
    app.router.add_get("/api/thumb/{segment}.jpg", self.thumb)
    app.router.add_get("/video/{segment}/{camera}.mp4", self.video)
    app.router.add_post("/api/alive", self.alive)
    return app


def stop_reason(params: Params, sm: messaging.SubMaster, server: MediaServer) -> str | None:
  if not params.get_bool("ShareVideos"):
    return "turned off"
  if sm.recv_frame['carState'] > 0 and sm['carState'].vEgo > MOVING_SPEED:
    return "car is moving"
  if server.idle_time() > IDLE_TIMEOUT:
    return "idle"
  return None


async def serve(server: MediaServer, params: Params, port: int = PORT) -> str:
  runner = web.AppRunner(server.make_app(), access_log=None, shutdown_timeout=SHUTDOWN_TIMEOUT)
  await runner.setup()
  await web.TCPSite(runner, "0.0.0.0", port).start()
  cloudlog.info(f"mediaserverd serving {server.log_root} on port {port}")

  sm = messaging.SubMaster(['carState'])
  try:
    while (reason := stop_reason(params, sm, server)) is None:
      await asyncio.sleep(CHECK_INTERVAL)
      sm.update(0)
  finally:
    # right away, so the UI turns the hotspot off without waiting for open transfers
    params.put_bool("ShareVideos", False, block=True)
    await runner.cleanup()
  return reason


def main() -> None:
  os.nice(10)  # stay out of the way of the driving processes
  params = Params()
  cache_dir = default_cache_dir()
  shutil.rmtree(cache_dir, ignore_errors=True)
  server = MediaServer(Paths.log_root(), cache_dir)
  try:
    reason = asyncio.run(serve(server, params))
    cloudlog.warning(f"mediaserverd stopping: {reason}")
  finally:
    shutil.rmtree(cache_dir, ignore_errors=True)
    params.put_bool("ShareVideos", False, block=True)


if __name__ == "__main__":
  main()
