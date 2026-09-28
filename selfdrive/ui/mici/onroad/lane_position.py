import pyray as rl

from openpilot.common.filter_simple import FirstOrderFilter
from openpilot.common.realtime import DT_MDL
from openpilot.selfdrive.ui.ui_state import ui_state
from openpilot.system.ui.lib.application import gui_app, FontWeight
from openpilot.system.ui.lib.text_measure import measure_text_cached
from openpilot.system.ui.widgets import Widget

MIN_LANE_LINE_PROB = 0.5
LANE_WIDTH_RANGE = (2.5, 4.5)  # meters, rejects lines that aren't the ego lane
LANE_POSITION_RC = 2.0  # seconds, averages out weaving
LANE_POSITION_TIMEOUT = 1.0  # seconds, hides the value when the lane lines are lost
PARAM_READ_INTERVAL = 1.0  # seconds
FONT_SIZE = 36
TEXT_COLOR = rl.Color(255, 255, 255, int(255 * 0.8))


def get_lane_position(model) -> float | None:
  """Distance of the car's centerline from the ego lane center in meters, positive to the right.

  Model positions are relative to the camera, or to the car's centerline when CameraOffset is set correctly.
  Returns None when both ego lane lines aren't clearly visible.
  """
  if len(model.laneLines) < 3 or len(model.laneLineProbs) < 3:
    return None
  left, right = model.laneLines[1], model.laneLines[2]
  if min(model.laneLineProbs[1], model.laneLineProbs[2]) < MIN_LANE_LINE_PROB or len(left.y) == 0 or len(right.y) == 0:
    return None
  if not LANE_WIDTH_RANGE[0] <= right.y[0] - left.y[0] <= LANE_WIDTH_RANGE[1]:
    return None
  # lane lines are at y[0] meters to the right, so the lane center is at their mean
  return -(left.y[0] + right.y[0]) / 2


def format_lane_position(lane_position: float) -> str:
  cm = round(abs(lane_position) * 100)
  return "lane center" if cm == 0 else f"{cm} cm {'right' if lane_position > 0 else 'left'}"


class LanePosition(Widget):
  """Shows how far the car drives from the lane center, when the ShowLanePosition toggle is on"""
  def __init__(self):
    super().__init__()
    self._font = gui_app.font(FontWeight.SEMI_BOLD)
    # updated with each model frame
    self._filter = FirstOrderFilter(0.0, LANE_POSITION_RC, DT_MDL)
    self._last_valid_time = -LANE_POSITION_TIMEOUT
    self._enabled = False
    self._last_param_read_time = -PARAM_READ_INTERVAL

  def _update_state(self):
    now = rl.get_time()
    if now - self._last_param_read_time >= PARAM_READ_INTERVAL:
      self._enabled = ui_state.params.get_bool("ShowLanePosition")
      self._last_param_read_time = now

    sm = ui_state.sm
    if not self._enabled or not sm.updated['modelV2']:
      return

    lane_position = get_lane_position(sm['modelV2'])
    if lane_position is not None:
      # start from the first value instead of 0 after the lines were lost
      if now - self._last_valid_time >= LANE_POSITION_TIMEOUT:
        self._filter.x = lane_position
      self._filter.update(lane_position)
      self._last_valid_time = now

  def _render(self, rect: rl.Rectangle):
    if not self._enabled or rl.get_time() - self._last_valid_time >= LANE_POSITION_TIMEOUT:
      return

    text = format_lane_position(self._filter.x)
    size = measure_text_cached(self._font, text, FONT_SIZE)
    pos = rl.Vector2(rect.x + (rect.width - size.x) / 2, rect.y + rect.height - size.y - 20)
    rl.draw_text_ex(self._font, text, pos, FONT_SIZE, 0, TEXT_COLOR)
