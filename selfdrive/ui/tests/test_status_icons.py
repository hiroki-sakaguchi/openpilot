from cereal import log, messaging
from openpilot.selfdrive.ui.mici.onroad.status_icons import THERMAL_COLORS, temperature_text, thermal_color

ThermalStatus = log.DeviceState.ThermalStatus


def test_temperature_text():
  assert temperature_text(62.4) == "62°C"
  assert temperature_text(91.6) == "92°C"


def test_thermal_color_from_message():
  for status in (ThermalStatus.ok, ThermalStatus.overheated, ThermalStatus.critical):
    msg = messaging.new_message('deviceState')
    msg.deviceState.thermalStatus = status
    for device_state in (msg.deviceState, msg.as_reader().deviceState):
      assert thermal_color(device_state.thermalStatus) == THERMAL_COLORS[status]
  assert thermal_color(ThermalStatus.overheated) != thermal_color(ThermalStatus.ok)
