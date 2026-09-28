from opendbc.car import Bus, create_button_events, structs

ButtonType = structs.CarState.ButtonEvent.Type

# PCM_CRUISE.CRUISE_STATE briefly reports these values when the cruise stalk is clicked up or down
CRUISE_STATE_TO_BUTTON = {
  9: ButtonType.accelCruise,
  10: ButtonType.decelCruise,
}


class ToyotaCruiseStalk:
  """Adds Toyota cruise stalk clicks to CarState as accelCruise/decelCruise button events.

  Toyota uses PCM cruise, so these events don't change openpilot's set speed.
  """
  def __init__(self):
    self.cruise_state = 0

  def update(self, CS: structs.CarState, can_parsers) -> None:
    # every value received since the last update, so a click shorter than one card step isn't missed
    cruise_states = can_parsers[Bus.pt].vl_all["PCM_CRUISE"]["CRUISE_STATE"]

    button_events = []
    for cruise_state in cruise_states:
      prev_cruise_state = self.cruise_state
      self.cruise_state = int(cruise_state)
      # only states in CRUISE_STATE_TO_BUTTON are buttons, the rest are unpressed
      cur = self.cruise_state if self.cruise_state in CRUISE_STATE_TO_BUTTON else 0
      prev = prev_cruise_state if prev_cruise_state in CRUISE_STATE_TO_BUTTON else 0
      button_events += create_button_events(cur, prev, CRUISE_STATE_TO_BUTTON)

    if button_events:
      # CS.buttonEvents is a capnp list, so it can't be extended in place
      existing = [structs.CarState.ButtonEvent(pressed=be.pressed, type=be.type) for be in CS.buttonEvents]
      CS.buttonEvents = existing + button_events
