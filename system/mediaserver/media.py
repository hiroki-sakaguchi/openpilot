"""Finds the recorded segments on the device and prepares their thumbnails and videos for mediaserverd"""
import io
import os
import re
from dataclasses import dataclass, field

import av
import zstandard as zstd

from cereal import log

SEGMENT_RE = re.compile(r"^(?P<route>.+)--(?P<num>\d+)$")

# camera name -> file in the segment directory
CAMERA_FILES = {
  "qcamera": "qcamera.ts",  # low resolution H.264 in MPEG-TS
  "fcamera": "fcamera.hevc",  # road camera, raw HEVC
  "ecamera": "ecamera.hevc",  # wide road camera, raw HEVC
  "dcamera": "dcamera.hevc",  # driver camera, raw HEVC, only when RecordFront is set
}
RAW_HEVC_FPS = 20
MIN_VALID_TIME = 1704067200  # 2024-01-01, the clock may not be set yet when a route starts


@dataclass
class Segment:
  name: str
  num: int
  cameras: list[str]


@dataclass
class Route:
  name: str
  segments: list[Segment] = field(default_factory=list)
  start_time: float | None = None  # unix time from the first segment's log


def list_routes(log_root: str) -> list[Route]:
  """Routes with their finished segments, newest first. Segments still being written have .lock files"""
  routes: dict[str, Route] = {}
  mtimes: dict[str, float] = {}
  for entry in os.scandir(log_root):
    m = SEGMENT_RE.match(entry.name)
    if m is None or not entry.is_dir():
      continue
    files = os.listdir(entry.path)
    if any(f.endswith(".lock") for f in files):
      continue
    cameras = [cam for cam, fn in CAMERA_FILES.items() if fn in files]
    if not cameras:
      continue
    route = routes.setdefault(m["route"], Route(m["route"]))
    route.segments.append(Segment(entry.name, int(m["num"]), cameras))
    mtimes[m["route"]] = max(mtimes.get(m["route"], 0.), entry.stat().st_mtime)

  for route in routes.values():
    route.segments.sort(key=lambda s: s.num)
    route.start_time = get_start_time(os.path.join(log_root, route.segments[0].name))
  return sorted(routes.values(), key=lambda r: mtimes[r.name], reverse=True)


def _log_path(segment_dir: str) -> str | None:
  for fn in ("qlog.zst", "rlog.zst"):
    path = os.path.join(segment_dir, fn)
    if os.path.isfile(path):
      return path
  return None


def _read_exact(reader, size: int) -> bytes:
  dat = b""
  while len(dat) < size and (chunk := reader.read(size - len(dat))):
    dat += chunk
  if len(dat) < size:
    raise EOFError
  return dat


def _read_first_message(reader) -> bytes:
  """One Cap'n Proto message: a segment table with the segment count and sizes in words, then the segments"""
  head = _read_exact(reader, 4)
  n_segments = int.from_bytes(head, "little") + 1
  table = head + _read_exact(reader, 4 * n_segments + (4 if n_segments % 2 == 0 else 0))  # padded to 8 bytes
  sizes = [int.from_bytes(table[4 + 4 * i:8 + 4 * i], "little") for i in range(n_segments)]
  return table + _read_exact(reader, 8 * sum(sizes))


def get_start_time(segment_dir: str) -> float | None:
  """Unix time the route started. loggerd writes the same initData at the start of every segment's log.
  Only decompresses the first event of the log."""
  path = _log_path(segment_dir)
  if path is None:
    return None
  try:
    with open(path, "rb") as f, zstd.ZstdDecompressor().stream_reader(f, read_across_frames=True) as reader:
      head = _read_first_message(reader)
    with log.Event.from_bytes(head) as event:
      if event.which() == "initData" and event.initData.wallTimeNanos / 1e9 > MIN_VALID_TIME:
        return event.initData.wallTimeNanos / 1e9
  except Exception:
    pass
  return None


def get_thumbnail(segment_dir: str) -> bytes | None:
  """JPEG of the segment. Uses the thumbnail loggerd logs once a minute, else decodes the first qcamera frame"""
  path = _log_path(segment_dir)
  if path is not None:
    try:
      with open(path, "rb") as f, zstd.ZstdDecompressor().stream_reader(f) as reader:
        dat = reader.read()
      for event in log.Event.read_multiple_bytes(dat):
        if event.which() == "thumbnail" and len(event.thumbnail.thumbnail):
          return bytes(event.thumbnail.thumbnail)
    except Exception:
      pass

  qcamera = os.path.join(segment_dir, CAMERA_FILES["qcamera"])
  if os.path.isfile(qcamera):
    try:
      with av.open(qcamera) as container:
        frame = next(container.decode(video=0))
      buf = io.BytesIO()
      frame.to_image().save(buf, format="JPEG", quality=80)
      return buf.getvalue()
    except Exception:
      pass
  return None


def remux_to_mp4(src: str, dst: str) -> None:
  """Copies a qcamera.ts (video and audio, if recorded) or raw .hevc file into an MP4 that phone browsers can play,
  without re-encoding"""
  raw_hevc = src.endswith(".hevc")
  tmp = dst + ".tmp"
  in_args = {"format": "hevc", "options": {"framerate": str(RAW_HEVC_FPS)}} if raw_hevc else {}
  with av.open(src, **in_args) as inp, av.open(tmp, "w", format="mp4", options={"movflags": "+faststart"}) as out:
    in_video = inp.streams.video[0]
    in_streams = [in_video] + ([] if raw_hevc else list(inp.streams.audio[:1]))
    out_streams = {s.index: out.add_stream_from_template(s) for s in in_streams}
    if raw_hevc:
      out_streams[in_video.index].codec_tag = "hvc1"  # needed for Safari to play HEVC

    # qcamera timestamps continue across segments, start each file at 0, keeping audio in sync
    start = (inp.start_time or 0) / av.time_base
    frame = 0
    for pkt in inp.demux(in_streams):
      if pkt.size == 0:  # flush packet
        continue
      in_stream = pkt.stream
      if raw_hevc or pkt.dts is None or pkt.pts is None:
        if in_stream is not in_video:
          continue
        # raw HEVC has no timestamps. Frames are evenly spaced, and in display order since loggerd doesn't use B-frames
        pkt.pts = pkt.dts = frame * (pkt.duration or round(1 / (RAW_HEVC_FPS * in_stream.time_base)))
        frame += 1
      else:
        offset = round(start / in_stream.time_base)
        pkt.dts -= offset
        pkt.pts -= offset
      pkt.stream = out_streams[in_stream.index]
      out.mux(pkt)
  os.replace(tmp, dst)
