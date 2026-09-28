import pytest

from openpilot.selfdrive.controls.lib.longitudinal_planner import ALLOW_THROTTLE_THRESHOLD, ALLOW_THROTTLE_THRESHOLDS, MIN_ALLOW_THROTTLE_SPEED, \
                                                                 get_allow_throttle_threshold
from openpilot.selfdrive.test.longitudinal_maneuvers.plant import Plant


def test_get_allow_throttle_threshold():
  assert get_allow_throttle_threshold(0) == ALLOW_THROTTLE_THRESHOLD
  for idx, threshold in enumerate(ALLOW_THROTTLE_THRESHOLDS):
    assert get_allow_throttle_threshold(idx) == threshold
  for invalid_idx in (None, -1, len(ALLOW_THROTTLE_THRESHOLDS)):
    assert get_allow_throttle_threshold(invalid_idx) == ALLOW_THROTTLE_THRESHOLD


@pytest.mark.parametrize("threshold", ALLOW_THROTTLE_THRESHOLDS)
def test_throttle_gating_threshold(threshold):
  plant = Plant(speed=15.0)
  plant.planner.allow_throttle_threshold = threshold
  # gas press probabilities from the model, between and around the thresholds
  for prob in (0.35, 0.45, 0.55, 0.65, 0.35):
    plant.step(v_cruise=30., prob_throttle=prob)
    assert plant.planner.allow_throttle == (prob > threshold), f"{prob=}"


def test_throttle_allowed_at_low_speed():
  plant = Plant(speed=MIN_ALLOW_THROTTLE_SPEED / 2)
  plant.planner.allow_throttle_threshold = ALLOW_THROTTLE_THRESHOLDS[-1]
  plant.step(v_cruise=30., prob_throttle=0.0)
  assert plant.planner.allow_throttle
