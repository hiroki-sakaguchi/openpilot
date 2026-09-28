import numpy as np

# Lateral distance of the road camera from the car's centerline in meters, positive when mounted left of center.
# Selectable values, indexed by the CameraOffset param
CAMERA_OFFSETS = (-0.20, -0.15, -0.10, -0.05, 0.0, 0.05, 0.10, 0.15, 0.20)
DEFAULT_CAMERA_OFFSET_IDX = CAMERA_OFFSETS.index(0.0)
# Time constant for applying a changed camera offset gradually, in seconds
CAMERA_OFFSET_RC = 1.0


def get_camera_offset(camera_offset_idx) -> float:
  if camera_offset_idx is None or not 0 <= camera_offset_idx < len(CAMERA_OFFSETS):
    return 0.0
  return CAMERA_OFFSETS[camera_offset_idx]


def get_v_horizon(intrinsics: np.ndarray, rpy_calib) -> float:
  """Image row of the horizon, with pitch positive when the camera points down"""
  cy = intrinsics[1, 2]
  if len(rpy_calib) == 3 and np.isfinite(rpy_calib).all():
    return float(cy - intrinsics[1, 1] * np.tan(rpy_calib[1]))
  return float(cy)


def apply_camera_offset(model_transform: np.ndarray, camera_offset: float, height: float, v_horizon: float) -> np.ndarray:
  """Make the model see the road as if the camera was on the car's centerline.

  model_transform maps model input pixels to camera pixels. The shear maps pixels of a camera on the
  centerline to the real camera, using the ground plane: a road point at image row v is shifted
  sideways by camera_offset / height * (v - v_horizon) pixels. The horizon stays fixed, so calibration
  isn't affected. Objects above the road aren't fully corrected, so offsets should stay small.
  """
  if camera_offset == 0.0:
    return model_transform
  shear = np.eye(3, dtype=np.float32)
  shear[0, 1] = camera_offset / height
  shear[0, 2] = -camera_offset / height * v_horizon
  return (shear @ model_transform).astype(np.float32)
