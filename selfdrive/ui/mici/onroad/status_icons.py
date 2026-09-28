import pyray as rl

from cereal import log
from openpilot.selfdrive.ui.ui_state import ui_state
from openpilot.system.ui.lib.application import gui_app, FontWeight
from openpilot.system.ui.lib.text_measure import measure_text_cached
from openpilot.system.ui.widgets import Widget

ThermalStatus = log.DeviceState.ThermalStatus

ICON_SIZE = 40
FONT_SIZE = 26
PADDING = 16
SPACING = 8
PARAM_READ_INTERVAL = 1.0  # seconds

# follows the device's own thermal status, which also drives the fan and throttling
THERMAL_COLORS = {
  ThermalStatus.ok: rl.Color(255, 255, 255, int(255 * 0.6)),
  ThermalStatus.overheated: rl.Color(255, 145, 0, 255),
  ThermalStatus.critical: rl.Color(255, 50, 50, 255),
}


def temperature_text(temp_c: float) -> str:
  return f"{round(temp_c)}°C"


def thermal_color(thermal_status) -> rl.Color:
  # capnp enums read from a message don't hash like their int values
  return THERMAL_COLORS.get(getattr(thermal_status, "raw", thermal_status), THERMAL_COLORS[ThermalStatus.ok])


class StatusIcons(Widget):
  """Top right of the onroad view: the experimental mode icon, and the device temperature when ShowDeviceTemp is set"""
  def __init__(self):
    super().__init__()
    self._txt_experimental = gui_app.texture("icons_mici/experimental_mode.png", ICON_SIZE, ICON_SIZE)
    self._font = gui_app.font(FontWeight.SEMI_BOLD)
    self._show_temp = False
    self._last_param_read_time = -PARAM_READ_INTERVAL

  def _update_state(self):
    now = rl.get_time()
    if now - self._last_param_read_time >= PARAM_READ_INTERVAL:
      self._show_temp = ui_state.params.get_bool("ShowDeviceTemp")
      self._last_param_read_time = now

  def _render(self, rect: rl.Rectangle):
    sm = ui_state.sm
    right = rect.x + rect.width - PADDING
    center_y = rect.y + PADDING + ICON_SIZE / 2

    # drawn right to left
    if self._show_temp and sm.recv_frame['deviceState'] > 0:
      device_state = sm['deviceState']
      text = temperature_text(device_state.maxTempC)
      size = measure_text_cached(self._font, text, FONT_SIZE)
      right -= size.x
      rl.draw_text_ex(self._font, text, rl.Vector2(right, center_y - size.y / 2), FONT_SIZE, 0, thermal_color(device_state.thermalStatus))
      right -= SPACING

    if sm['selfdriveState'].experimentalMode:
      right -= ICON_SIZE
      rl.draw_texture_ex(self._txt_experimental, rl.Vector2(right, center_y - ICON_SIZE / 2), 0.0, 1.0, rl.WHITE)
