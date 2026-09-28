from types import SimpleNamespace

import pytest

from openpilot.selfdrive.modeld.camera_offset import CAMERA_OFFSETS
from openpilot.selfdrive.ui.mici.layouts.settings.toggles import camera_offset_label
from openpilot.selfdrive.ui.mici.onroad.lane_position import format_lane_position, get_lane_position


def make_model(left_y, right_y, probs=(0.1, 0.9, 0.9, 0.1)):
  lines = [SimpleNamespace(y=[y]) for y in (left_y - 3.5, left_y, right_y, right_y + 3.5)]
  return SimpleNamespace(laneLines=lines, laneLineProbs=list(probs))


class TestLanePosition:
  def test_centered(self):
    assert get_lane_position(make_model(-1.75, 1.75)) == pytest.approx(0.0)

  def test_left_of_center(self):
    # the lane center is 0.15 m to the right, so the car is 0.15 m left of it
    assert get_lane_position(make_model(-1.6, 1.9)) == pytest.approx(-0.15)

  def test_right_of_center(self):
    assert get_lane_position(make_model(-1.9, 1.6)) == pytest.approx(0.15)

  def test_unclear_lines(self):
    assert get_lane_position(make_model(-1.75, 1.75, probs=(0.1, 0.9, 0.3, 0.1))) is None

  def test_implausible_lane_width(self):
    assert get_lane_position(make_model(-1.0, 1.0)) is None
    assert get_lane_position(make_model(-3.0, 3.0)) is None

  def test_missing_lines(self):
    assert get_lane_position(SimpleNamespace(laneLines=[], laneLineProbs=[])) is None

  def test_format(self):
    assert format_lane_position(0.004) == "lane center"
    assert format_lane_position(-0.15) == "15 cm left"
    assert format_lane_position(0.034) == "3 cm right"


def test_camera_offset_labels():
  labels = [camera_offset_label(o) for o in CAMERA_OFFSETS]
  assert labels[0] == "20 cm right"
  assert labels[CAMERA_OFFSETS.index(0.0)] == "center"
  assert labels[-1] == "20 cm left"
  assert len(set(labels)) == len(labels)
