import itertools
import numpy as np
import pytest

from openpilot.common.transformations.camera import DEVICE_CAMERAS, view_frame_from_device_frame
from openpilot.common.transformations.model import get_warp_matrix, medmodel_frame_from_calib_frame, sbigmodel_frame_from_calib_frame
from openpilot.common.transformations.orientation import rot_from_euler
from openpilot.selfdrive.modeld.camera_offset import CAMERA_OFFSETS, DEFAULT_CAMERA_OFFSET_IDX, apply_camera_offset, get_camera_offset, \
                                                     get_v_horizon

MICI_CAMERAS = DEVICE_CAMERAS[('mici', 'os04c10')]
HEIGHT = 1.22


def project(m, pt):
  p = m @ pt
  return p[:2] / p[2]


def road_points():
  # points on the road, relative to the car's centerline in the calibrated frame (x forward, y right, z down)
  return [np.array([x, y, HEIGHT]) for x, y in itertools.product([6., 10., 20., 40., 80.], [-3.5, -1.75, 0., 1.75, 3.5])]


def test_get_camera_offset():
  assert get_camera_offset(DEFAULT_CAMERA_OFFSET_IDX) == 0.0
  for idx, offset in enumerate(CAMERA_OFFSETS):
    assert get_camera_offset(idx) == offset
  for invalid_idx in (None, -1, len(CAMERA_OFFSETS)):
    assert get_camera_offset(invalid_idx) == 0.0


def test_no_offset_is_unchanged():
  transform = get_warp_matrix(np.array([0., 0.05, 0.02]), MICI_CAMERAS.fcam.intrinsics).astype(np.float32)
  np.testing.assert_array_equal(apply_camera_offset(transform, 0.0, HEIGHT, 100.), transform)


def test_v_horizon():
  intrinsics = MICI_CAMERAS.fcam.intrinsics
  assert get_v_horizon(intrinsics, []) == intrinsics[1, 2]
  for pitch in (-0.1, 0.0, 0.1):
    # a point far ahead on the horizon projects to the horizon row
    camera_from_calib = intrinsics @ view_frame_from_device_frame @ rot_from_euler([0., pitch, 0.])
    assert project(camera_from_calib, np.array([1e6, 0., 0.]))[1] == pytest.approx(get_v_horizon(intrinsics, [0., pitch, 0.]), abs=1e-3)


@pytest.mark.parametrize("camera_offset", [0.2, 0.15, -0.1])
@pytest.mark.parametrize("rpy", [[0., 0., 0.], [0., 0.06, 0.02], [0., -0.04, -0.03]])
@pytest.mark.parametrize("wide", [False, True])
def test_road_seen_from_centerline(camera_offset, rpy, wide):
  """With the offset applied, the model sees the road as a camera on the car's centerline would"""
  intrinsics = MICI_CAMERAS.ecam.intrinsics if wide else MICI_CAMERAS.fcam.intrinsics
  model_from_calib = (sbigmodel_frame_from_calib_frame if wide else medmodel_frame_from_calib_frame)[:, :3]
  rpy = np.array(rpy)

  transform = get_warp_matrix(rpy, intrinsics, wide)
  corrected = apply_camera_offset(transform, camera_offset, HEIGHT, get_v_horizon(intrinsics, rpy))
  camera_from_calib = intrinsics @ view_frame_from_device_frame @ rot_from_euler(rpy)

  for pt in road_points():
    model_px = np.append(project(model_from_calib, pt), 1.)
    # the real camera is camera_offset to the left of the centerline, so the point is that much further right from it
    real_camera_px = project(camera_from_calib, pt + np.array([0., camera_offset, 0.]))
    assert project(corrected, model_px) == pytest.approx(real_camera_px, abs=1.0)
    # without the correction, near points are clearly off
    if pt[0] < 10:
      assert np.abs(project(transform, model_px) - real_camera_px).max() > 5.0
