from types import SimpleNamespace

from opendbc.car import Bus, structs
from openpilot.selfdrive.car.toyota_cruise_stalk import ToyotaCruiseStalk

ButtonType = structs.CarState.ButtonEvent.Type


def update(stalk, cruise_states):
  can_parsers = {Bus.pt: SimpleNamespace(vl_all={"PCM_CRUISE": {"CRUISE_STATE": cruise_states}})}
  return [(be.type, be.pressed) for be in stalk.update(can_parsers)]


class TestToyotaCruiseStalk:
  def test_click_up_and_down(self):
    stalk = ToyotaCruiseStalk()
    assert update(stalk, [8.]) == []
    assert update(stalk, [9.]) == [(ButtonType.accelCruise, True)]
    assert update(stalk, [8.]) == [(ButtonType.accelCruise, False)]
    assert update(stalk, [10.]) == [(ButtonType.decelCruise, True)]
    assert update(stalk, [8.]) == [(ButtonType.decelCruise, False)]

  def test_click_within_one_update(self):
    # a click that starts and ends between two updates is not missed
    stalk = ToyotaCruiseStalk()
    assert update(stalk, [8., 10., 8.]) == [(ButtonType.decelCruise, True), (ButtonType.decelCruise, False)]

  def test_held_click(self):
    stalk = ToyotaCruiseStalk()
    assert update(stalk, [9., 9.]) == [(ButtonType.accelCruise, True)]
    assert update(stalk, [9.]) == []

  def test_no_new_messages(self):
    stalk = ToyotaCruiseStalk()
    assert update(stalk, [10.]) == [(ButtonType.decelCruise, True)]
    assert update(stalk, []) == []

  def test_other_states_are_not_buttons(self):
    stalk = ToyotaCruiseStalk()
    assert update(stalk, [0., 7., 8., 11., 1.]) == []
