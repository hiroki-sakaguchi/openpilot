#!/usr/bin/env python3
"""Replays modeld on the model replay CI segment with CameraOffset at 0 and 0.15 m, and compares the ego lane center.

CI never runs modeld (process replay excludes it), so run this after changing selfdrive/modeld/. See FORK_CI.md.
Download the segment first (urllib3 range reads are very slow), then pass its directory:
  B=https://commadataci.blob.core.windows.net/openpilotci/8494c69d3c710e81/000001d4--2648a9a404/4
  mkdir -p /tmp/op_route && (cd /tmp/op_route && for f in rlog.zst fcamera.hevc ecamera.hevc; do curl -sO $B/$f; done)
  tools/fork/modeld_camera_offset_check.py /tmp/op_route
"""
import sys
import numpy as np

from openpilot.selfdrive.modeld.camera_offset import CAMERA_OFFSETS
from openpilot.selfdrive.test.process_replay.model_replay import START_FRAME, END_FRAME, trim_logs
from openpilot.selfdrive.test.process_replay.process_replay import get_process_config, replay_process
from openpilot.tools.lib.framereader import FrameReader
from openpilot.tools.lib.logreader import LogReader

CHECKED_OFFSET = 0.15


def replay_modeld(logs, frs, camera_offset: float):
  out = replay_process(get_process_config("modeld"), logs, frs, custom_params={"CameraOffset": CAMERA_OFFSETS.index(camera_offset)})
  models = [m.modelV2 for m in out if m.which() == 'modelV2']
  # ego lane center at the nearest point, positive right
  lane_centers = np.array([(m.laneLines[1].y[0] + m.laneLines[2].y[0]) / 2 for m in models])
  return len(models), lane_centers[-20:].mean()


def main(route_dir: str):
  lr = list(LogReader(f"{route_dir}/rlog.zst"))
  frs = {
    'roadCameraState': FrameReader(f"{route_dir}/fcamera.hevc", pix_fmt='nv12', cache_size=END_FRAME - START_FRAME),
    'wideRoadCameraState': FrameReader(f"{route_dir}/ecamera.hevc", pix_fmt='nv12', cache_size=END_FRAME - START_FRAME),
  }
  # same inputs as model_replay.model_replay
  logs = trim_logs(lr, START_FRAME, END_FRAME, {"roadCameraState", "wideRoadCameraState"},
                   {"roadEncodeIdx", "wideRoadEncodeIdx", "carParams", "carState", "carControl", "can"})
  for s in ('liveCalibration', 'deviceState'):
    msg = next(m for m in lr if m.which() == s).as_builder()
    msg.logMonoTime = lr[0].logMonoTime
    logs.insert(1, msg.as_reader())

  n0, center0 = replay_modeld(logs, frs, 0.0)
  n1, center1 = replay_modeld(logs, frs, CHECKED_OFFSET)
  print(f"offset 0 m: {n0} modelV2 msgs, lane center {center0:+.3f} m")
  print(f"offset {CHECKED_OFFSET} m: {n1} modelV2 msgs, lane center {center1:+.3f} m")
  # the model now sees from CHECKED_OFFSET to the right of the camera, so the lane center moves left by about that much
  print(f"shift {center1 - center0:+.3f} m, expected about {-CHECKED_OFFSET:+.3f} m")


if __name__ == "__main__":
  main(sys.argv[1] if len(sys.argv) > 1 else "/tmp/op_route")
