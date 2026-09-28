from cereal import log

from openpilot.system.ui.widgets.scroller import NavScroller
from openpilot.selfdrive.modeld.camera_offset import CAMERA_OFFSETS
from openpilot.selfdrive.ui.mici.widgets.button import BigParamControl, BigMultiParamToggle, BigCycleParamButton
from openpilot.system.ui.lib.application import gui_app
from openpilot.selfdrive.ui.layouts.settings.common import restart_needed_callback
from openpilot.selfdrive.ui.ui_state import ui_state

PERSONALITY_TO_INT = log.LongitudinalPersonality.schema.enumerants


def camera_offset_label(camera_offset: float) -> str:
  cm = round(abs(camera_offset) * 100)
  return "center" if cm == 0 else f"{cm} cm {'left' if camera_offset > 0 else 'right'}"


class TogglesLayoutMici(NavScroller):
  def __init__(self):
    super().__init__()

    self._personality_toggle = BigMultiParamToggle("driving personality", "LongitudinalPersonality", ["aggressive", "standard", "relaxed"])
    # options follow STOP_DISTANCES in long_mpc.py
    self._stop_distance_toggle = BigMultiParamToggle("stop distance", "StopDistance", ["6 m", "8 m", "10 m"])
    # how early the throttle is stopped when the model expects a slowdown. Options follow ALLOW_THROTTLE_THRESHOLDS in longitudinal_planner.py
    self._throttle_gating_toggle = BigMultiParamToggle("coast before stops", "ThrottleGating", ["stock", "early", "earlier"])
    self._experimental_btn = BigParamControl("experimental mode", "ExperimentalMode")
    # where the road camera is mounted, relative to the car's centerline
    self._camera_offset_btn = BigCycleParamButton("camera offset", "CameraOffset", [camera_offset_label(o) for o in CAMERA_OFFSETS])
    show_lane_position_toggle = BigParamControl("show lane position", "ShowLanePosition")
    is_metric_toggle = BigParamControl("use metric units", "IsMetric")
    ldw_toggle = BigParamControl("lane departure warnings", "IsLdwEnabled")
    always_on_dm_toggle = BigParamControl("always-on driver monitor", "AlwaysOnDM")
    record_front = BigParamControl("record & upload driver camera", "RecordFront", toggle_callback=restart_needed_callback)
    record_mic = BigParamControl("record & upload mic audio", "RecordAudio", toggle_callback=restart_needed_callback)
    enable_openpilot = BigParamControl("enable openpilot", "OpenpilotEnabledToggle", toggle_callback=restart_needed_callback)

    self._scroller.add_widgets([
      self._personality_toggle,
      self._stop_distance_toggle,
      self._throttle_gating_toggle,
      self._experimental_btn,
      self._camera_offset_btn,
      show_lane_position_toggle,
      is_metric_toggle,
      ldw_toggle,
      always_on_dm_toggle,
      record_front,
      record_mic,
      enable_openpilot,
    ])

    # Toggle lists
    self._refresh_toggles = (
      ("ExperimentalMode", self._experimental_btn),
      ("ShowLanePosition", show_lane_position_toggle),
      ("IsMetric", is_metric_toggle),
      ("IsLdwEnabled", ldw_toggle),
      ("AlwaysOnDM", always_on_dm_toggle),
      ("RecordFront", record_front),
      ("RecordAudio", record_mic),
      ("OpenpilotEnabledToggle", enable_openpilot),
    )

    enable_openpilot.set_enabled(lambda: not ui_state.engaged)
    record_front.set_enabled(False if ui_state.params.get_bool("RecordFrontLock") else (lambda: not ui_state.engaged))
    record_mic.set_enabled(lambda: not ui_state.engaged)

    if ui_state.params.get_bool("ShowDebugInfo"):
      gui_app.set_show_touches(True)
      gui_app.set_show_fps(True)

    ui_state.add_engaged_transition_callback(self._update_toggles)

  def _update_state(self):
    super()._update_state()

    if ui_state.sm.updated["selfdriveState"]:
      personality = PERSONALITY_TO_INT[ui_state.sm["selfdriveState"].personality]
      if personality != ui_state.personality and ui_state.started:
        self._personality_toggle.set_value(self._personality_toggle._options[personality])
      ui_state.personality = personality

  def show_event(self):
    super().show_event()
    self._update_toggles()

  def _update_toggles(self):
    ui_state.update_params()

    # CP gating for experimental mode
    if ui_state.CP is not None:
      if ui_state.has_longitudinal_control:
        self._experimental_btn.set_visible(True)
        self._personality_toggle.set_visible(True)
        self._stop_distance_toggle.set_visible(True)
        self._throttle_gating_toggle.set_visible(True)
      else:
        # no long for now
        self._experimental_btn.set_visible(False)
        self._experimental_btn.set_checked(False)
        self._personality_toggle.set_visible(False)
        self._stop_distance_toggle.set_visible(False)
        self._throttle_gating_toggle.set_visible(False)
        ui_state.params.remove("ExperimentalMode")

    # Refresh toggles from params to mirror external changes
    for key, item in self._refresh_toggles:
      item.set_checked(ui_state.params.get_bool(key))
    self._camera_offset_btn.refresh()
