import asyncio
import io
import os
import socket
import time
from fractions import Fraction
from types import SimpleNamespace

import av
import numpy as np
import pytest
import zstandard as zstd
from aiohttp import ClientError, ClientSession, web
from aiohttp.test_utils import TestClient, TestServer
from PIL import Image

from cereal import log
from openpilot.common.params import Params
from openpilot.system.mediaserver import mediaserverd
from openpilot.system.mediaserver.media import get_start_time, get_thumbnail, list_routes, remux_to_mp4

FPS = 20
FRAMES = 10
ROUTE_START = 1759000000.0  # 2025-09-27


def has_encoder(name: str) -> bool:
  try:
    av.codec.Codec(name, "w")
    return True
  except Exception:
    return False


needs_h264 = pytest.mark.skipif(not has_encoder("libx264"), reason="no H.264 encoder")
needs_hevc = pytest.mark.skipif(not has_encoder("libx265"), reason="no HEVC encoder")


def jpeg(color) -> bytes:
  buf = io.BytesIO()
  Image.new("RGB", (32, 20), color).save(buf, format="JPEG")
  return buf.getvalue()


def write_log(path: str, wall_time: float | None, thumbnail: bytes | None, init_padding: int = 0) -> None:
  events = []
  if wall_time is not None:
    init = log.Event.new_message()
    init.init("initData")
    init.initData.wallTimeNanos = int(wall_time * 1e9)
    # initData also holds params and hardware info, so its size isn't bounded
    init.initData.kernelArgs = ["x" * 1000] * (init_padding // 1000)
    events.append(init)
  if thumbnail is not None:
    thumb = log.Event.new_message()
    thumb.init("thumbnail")
    thumb.thumbnail.thumbnail = thumbnail
    events.append(thumb)
  with open(path, "wb") as f:
    f.write(zstd.compress(b"".join(e.to_bytes() for e in events)))


def write_video(path: str, codec: str, audio: bool = False) -> None:
  """Like the device: qcamera.ts is H.264 in MPEG-TS with timestamps and AAC audio when RecordAudio is set,
  .hevc files are raw HEVC without timestamps"""
  raw = path.endswith(".hevc")
  with av.open(path, "w", format="hevc" if raw else "mpegts") as out:
    # all streams have to exist before muxing starts
    stream = out.add_stream(codec, rate=FPS)
    stream.width, stream.height, stream.pix_fmt = 64, 48, "yuv420p"
    stream.time_base = Fraction(1, FPS)
    # no B-frames, like loggerd's encoders
    stream.options = {"bf": "0"} if codec == "libx264" else {"x265-params": "bframes=0:log-level=error"}
    audio_stream = out.add_stream("aac", rate=16000, layout="mono") if audio else None

    for i in range(FRAMES):
      frame = av.VideoFrame.from_image(Image.new("RGB", (64, 48), (i * 20, 0, 0)))
      frame.pts, frame.time_base = 1000 + i, Fraction(1, FPS)
      for pkt in stream.encode(frame):
        out.mux(pkt)
    for pkt in stream.encode():
      out.mux(pkt)

    if audio_stream is not None:
      # starts with the video, at 1000 / FPS s
      for i in range(FRAMES // 2):
        audio_frame = av.AudioFrame.from_ndarray(np.zeros((1, 1024), dtype=np.float32), format="fltp", layout="mono")
        audio_frame.sample_rate, audio_frame.pts = 16000, 1000 // FPS * 16000 + i * 1024
        for pkt in audio_stream.encode(audio_frame):
          out.mux(pkt)
      for pkt in audio_stream.encode():
        out.mux(pkt)


def make_segment(root, name: str, files: dict[str, bytes] | None = None, mtime: float | None = None) -> str:
  path = os.path.join(root, name)
  os.makedirs(path)
  for fn, dat in (files or {}).items():
    with open(os.path.join(path, fn), "wb") as f:
      f.write(dat)
  if mtime is not None:
    os.utime(path, (mtime, mtime))
  return path


class TestMedia:
  def test_list_routes(self, tmp_path):
    video = {"qcamera.ts": b"x"}
    seg = make_segment(tmp_path, "0000000a--aaaaaaaaaa--2", {**video, "fcamera.hevc": b"x"})
    write_log(os.path.join(seg, "qlog.zst"), ROUTE_START, None)
    os.utime(seg, (100, 100))
    make_segment(tmp_path, "0000000a--aaaaaaaaaa--3", video, mtime=160)
    make_segment(tmp_path, "0000000a--aaaaaaaaaa--4", {**video, "qcamera.ts.lock": b""}, mtime=220)  # still recording
    make_segment(tmp_path, "0000000b--bbbbbbbbbb--0", video, mtime=500)
    make_segment(tmp_path, "0000000c--cccccccccc--0", {"qlog.zst": b"x"}, mtime=600)  # no videos
    make_segment(tmp_path, "not-a-segment", video)

    routes = list_routes(str(tmp_path))
    assert [r.name for r in routes] == ["0000000b--bbbbbbbbbb", "0000000a--aaaaaaaaaa"]  # newest first
    route = routes[1]
    assert [(s.num, s.cameras) for s in route.segments] == [(2, ["qcamera", "fcamera"]), (3, ["qcamera"])]
    # every segment's log has the route's start time, also when older segments were deleted
    assert route.start_time == pytest.approx(ROUTE_START)
    assert routes[0].start_time is None

  def test_start_time(self, tmp_path):
    seg = make_segment(tmp_path, "r--0")
    assert get_start_time(seg) is None
    write_log(os.path.join(seg, "rlog.zst"), 1234.5, None)
    assert get_start_time(seg) is None  # clock not set yet
    write_log(os.path.join(seg, "rlog.zst"), ROUTE_START, None)
    assert get_start_time(seg) == pytest.approx(ROUTE_START)

  def test_start_time_large_init_data(self, tmp_path):
    seg = make_segment(tmp_path, "r--0")
    write_log(os.path.join(seg, "qlog.zst"), ROUTE_START, jpeg("red"), init_padding=200_000)
    assert get_start_time(seg) == pytest.approx(ROUTE_START)

  def test_thumbnail_from_log(self, tmp_path):
    seg = make_segment(tmp_path, "r--0")
    thumbnail = jpeg("red")
    write_log(os.path.join(seg, "qlog.zst"), 1.0, thumbnail)
    assert get_thumbnail(seg) == thumbnail

  @needs_h264
  def test_thumbnail_from_qcamera(self, tmp_path):
    seg = make_segment(tmp_path, "r--0")
    write_log(os.path.join(seg, "qlog.zst"), 1.0, None)
    write_video(os.path.join(seg, "qcamera.ts"), "libx264")
    thumbnail = get_thumbnail(seg)
    assert Image.open(io.BytesIO(thumbnail)).size == (64, 48)

  def test_no_thumbnail(self, tmp_path):
    assert get_thumbnail(make_segment(tmp_path, "r--0")) is None

  @pytest.mark.parametrize("fn, codec, tag", [
    pytest.param("qcamera.ts", "libx264", "avc1", marks=needs_h264),
    pytest.param("fcamera.hevc", "libx265", "hvc1", marks=needs_hevc),
  ])
  def test_remux_to_mp4(self, tmp_path, fn, codec, tag):
    src, dst = str(tmp_path / fn), str(tmp_path / "out.mp4")
    write_video(src, codec)
    remux_to_mp4(src, dst)
    with av.open(dst) as c:
      stream = c.streams.video[0]
      assert stream.codec_tag == tag
      frames = list(c.decode(stream))
    assert len(frames) == FRAMES
    # starts at 0 and frames are 1/FPS apart
    assert [float(f.time) for f in frames] == pytest.approx([i / FPS for i in range(FRAMES)])
    assert not os.path.exists(dst + ".tmp")

  @needs_h264
  def test_remux_keeps_audio(self, tmp_path):
    src, dst = str(tmp_path / "qcamera.ts"), str(tmp_path / "out.mp4")
    write_video(src, "libx264", audio=True)
    remux_to_mp4(src, dst)
    with av.open(dst) as c:
      assert c.streams.audio[0].codec_context.name == "aac"
      audio_start = float(next(c.decode(audio=0)).time)
    with av.open(dst) as c:
      video_start = float(next(c.decode(video=0)).time)
    # both start near 0, like in the original file
    assert audio_start < 0.2 and video_start < 0.2


class TestMediaServer:
  def test_is_allowed(self):
    for ip in ("192.168.43.1", "192.168.43.25", "127.0.0.1", "::1"):
      assert mediaserverd.is_allowed(ip), ip
    for ip in ("192.168.1.10", "10.0.0.2", "8.8.8.8", "", None, "garbage"):
      assert not mediaserverd.is_allowed(ip), ip

  @needs_h264
  def test_endpoints(self, tmp_path):
    root = tmp_path / "realdata"
    seg = make_segment(root, "0000000a--aaaaaaaaaa--0")
    write_log(os.path.join(seg, "qlog.zst"), ROUTE_START, jpeg("blue"))
    write_video(os.path.join(seg, "qcamera.ts"), "libx264")
    make_segment(root, "0000000a--aaaaaaaaaa--1", {"qcamera.ts": b"", "qcamera.ts.lock": b""})
    server = mediaserverd.MediaServer(str(root), str(tmp_path / "cache"))

    async def run():
      async with TestClient(TestServer(server.make_app())) as client:
        r = await client.get("/")
        assert r.status == 200 and "video" in await r.text()

        r = await client.get("/api/routes")
        routes = await r.json()
        assert [s["name"] for s in routes[0]["segments"]] == ["0000000a--aaaaaaaaaa--0"]
        assert routes[0]["start"] == pytest.approx(ROUTE_START)

        r = await client.get("/api/thumb/0000000a--aaaaaaaaaa--0.jpg")
        assert r.status == 200 and r.content_type == "image/jpeg"

        r = await client.get("/video/0000000a--aaaaaaaaaa--0/qcamera.mp4")
        assert r.status == 200 and r.content_type == "video/mp4"
        assert "Content-Disposition" not in r.headers
        r = await client.get("/video/0000000a--aaaaaaaaaa--0/qcamera.mp4?dl=1")
        assert "attachment" in r.headers["Content-Disposition"]
        r = await client.get("/video/0000000a--aaaaaaaaaa--0/qcamera.mp4", headers={"Range": "bytes=0-9"})
        assert r.status == 206

        for url in ("/video/0000000a--aaaaaaaaaa--1/qcamera.mp4",  # still recording
                    "/video/0000000a--aaaaaaaaaa--0/fcamera.mp4",  # not recorded
                    "/video/0000000a--aaaaaaaaaa--0/passwd.mp4",
                    "/video/..%2F..%2Fetc/qcamera.mp4",
                    "/api/thumb/0000000a--aaaaaaaaaa--9.jpg"):
          assert (await client.get(url)).status == 404, url

        assert (await client.post("/api/alive")).status == 204
    asyncio.run(run())

  def test_requests_keep_it_alive(self, tmp_path):
    server = mediaserverd.MediaServer(str(tmp_path), str(tmp_path / "cache"))
    server.last_activity -= 100

    async def run():
      async with TestClient(TestServer(server.make_app())) as client:
        await client.post("/api/alive")
    asyncio.run(run())
    assert server.idle_time() < 5


class TestStopping:
  def test_stop_reason(self, tmp_path):
    params = Params()
    params.put_bool("ShareVideos", True, block=True)
    server = mediaserverd.MediaServer(str(tmp_path), str(tmp_path / "cache"))

    class SM(dict):
      def __init__(self, v_ego):
        super().__init__(carState=SimpleNamespace(vEgo=v_ego or 0.))
        self.recv_frame = {'carState': 0 if v_ego is None else 1}

    assert mediaserverd.stop_reason(params, SM(None), server) is None  # offroad, no carState
    assert mediaserverd.stop_reason(params, SM(0.3), server) is None  # stopped
    assert mediaserverd.stop_reason(params, SM(5.0), server) == "car is moving"
    server.last_activity -= mediaserverd.IDLE_TIMEOUT + 1
    assert mediaserverd.stop_reason(params, SM(0.0), server) == "idle"
    params.put_bool("ShareVideos", False, block=True)
    assert mediaserverd.stop_reason(params, SM(0.0), server) == "turned off"

  def test_stops_during_transfer(self, tmp_path, monkeypatch):
    """Open transfers are cut off when it stops, e.g. when the car starts moving during a download"""
    monkeypatch.setattr(mediaserverd, "CHECK_INTERVAL", 0.05)
    params = Params()
    params.put_bool("ShareVideos", True, block=True)
    server = mediaserverd.MediaServer(str(tmp_path), str(tmp_path / "cache"))

    async def slow_download(request):
      response = web.StreamResponse()
      await response.prepare(request)
      while True:
        await response.write(b"x" * 1024)
        await asyncio.sleep(0.05)
    monkeypatch.setattr(server, "routes", slow_download)

    with socket.socket() as s:
      s.bind(("127.0.0.1", 0))
      port = s.getsockname()[1]

    async def run():
      serving = asyncio.create_task(mediaserverd.serve(server, params, port=port))
      async with ClientSession() as session:
        for _ in range(50):  # wait for the server to start
          try:
            await (await session.post(f"http://127.0.0.1:{port}/api/alive")).release()
            break
          except ClientError:
            await asyncio.sleep(0.05)
        async with session.get(f"http://127.0.0.1:{port}/api/routes") as response:
          await response.content.read(1024)
          params.put_bool("ShareVideos", False, block=True)  # stop while downloading
          start = time.monotonic()
          with pytest.raises(ClientError):
            await asyncio.wait_for(response.content.read(), timeout=5)
      reason = await asyncio.wait_for(serving, timeout=5)
      return reason, time.monotonic() - start
    reason, stop_time = asyncio.run(run())
    assert reason == "turned off"
    assert stop_time < 2.0

  @pytest.mark.parametrize("reason", ["idle", "turned off"])
  def test_serve_stops(self, tmp_path, monkeypatch, reason):
    monkeypatch.setattr(mediaserverd, "CHECK_INTERVAL", 0.05)
    monkeypatch.setattr(mediaserverd, "IDLE_TIMEOUT", 0.3 if reason == "idle" else 60)
    params = Params()
    params.put_bool("ShareVideos", reason != "turned off", block=True)
    server = mediaserverd.MediaServer(str(tmp_path), str(tmp_path / "cache"))
    assert asyncio.run(asyncio.wait_for(mediaserverd.serve(server, params, port=0), timeout=10)) == reason
