from cereal import car
from openpilot.common.realtime import DT_CTRL
from openpilot.selfdrive.selfdrived.stalk_toggle import ExperimentalModeStalkToggle, TOGGLE_WINDOW

ButtonType = car.CarState.ButtonEvent.Type
UP, DOWN = ButtonType.accelCruise, ButtonType.decelCruise


def make_cs(buttons=(), enabled=True, standstill=False):
  CS = car.CarState.new_message()
  CS.cruiseState.enabled = enabled
  CS.cruiseState.standstill = standstill
  CS.buttonEvents = [car.CarState.ButtonEvent(type=b, pressed=pressed) for b in buttons for pressed in (True, False)]
  return CS


def run(toggle, steps):
  """steps: list of (buttons, frames to idle afterwards), returns number of toggles"""
  toggles = 0
  for buttons, idle_frames in steps:
    toggles += toggle.update(make_cs(buttons))
    for _ in range(idle_frames):
      toggles += toggle.update(make_cs())
  return toggles


class TestExperimentalModeStalkToggle:
  def test_down_up_down(self):
    assert run(ExperimentalModeStalkToggle(), [([DOWN], 50), ([UP], 50), ([DOWN], 0)]) == 1

  def test_wrong_sequence(self):
    for seq in ([UP, DOWN, UP], [DOWN, DOWN, UP], [DOWN, UP, UP], [DOWN, UP]):
      assert run(ExperimentalModeStalkToggle(), [([b], 20) for b in seq]) == 0

  def test_sequence_after_other_clicks(self):
    assert run(ExperimentalModeStalkToggle(), [([UP], 20), ([DOWN], 20), ([UP], 20), ([DOWN], 0)]) == 1

  def test_too_slow(self):
    gap = int(TOGGLE_WINDOW / DT_CTRL / 2) + 1
    assert run(ExperimentalModeStalkToggle(), [([DOWN], gap), ([UP], gap), ([DOWN], 0)]) == 0

  def test_repeated_toggles(self):
    toggle = ExperimentalModeStalkToggle()
    assert run(toggle, [([DOWN], 20), ([UP], 20), ([DOWN], 20)]) == 1
    # the finishing click isn't reused for the next sequence
    assert run(toggle, [([UP], 20), ([DOWN], 20)]) == 0
    assert run(toggle, [([DOWN], 20), ([UP], 20), ([DOWN], 20)]) == 1

  def test_only_while_cruising(self):
    for kwargs in ({"enabled": False}, {"standstill": True}):
      toggle = ExperimentalModeStalkToggle()
      assert sum(toggle.update(make_cs([b], **kwargs)) for b in (DOWN, UP, DOWN)) == 0

  def test_disengage_resets_sequence(self):
    toggle = ExperimentalModeStalkToggle()
    assert run(toggle, [([DOWN], 20), ([UP], 20)]) == 0
    toggle.update(make_cs(enabled=False))
    assert run(toggle, [([DOWN], 0)]) == 0
