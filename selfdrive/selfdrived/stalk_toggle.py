from collections import deque

from cereal import car
from openpilot.common.realtime import DT_CTRL

ButtonType = car.CarState.ButtonEvent.Type

# Cruise stalk down, up, down toggles experimental mode
TOGGLE_SEQUENCE = (ButtonType.decelCruise, ButtonType.accelCruise, ButtonType.decelCruise)
TOGGLE_WINDOW = 3.0  # seconds from the first to the last click


class ExperimentalModeStalkToggle:
  def __init__(self):
    self.frame = 0
    self.clicks: deque[tuple[int, int]] = deque(maxlen=len(TOGGLE_SEQUENCE))  # (button type, frame)

  def update(self, CS: car.CarState) -> bool:
    """Returns True when the toggle sequence was completed on this step"""
    self.frame += 1

    # The stalk also changes the set speed, and engages or resumes cruise,
    # so only accept the sequence while cruising
    if not CS.cruiseState.enabled or CS.cruiseState.standstill:
      self.clicks.clear()
      return False

    for be in CS.buttonEvents:
      if be.pressed and be.type in (ButtonType.accelCruise, ButtonType.decelCruise):
        self.clicks.append((be.type, self.frame))

    if tuple(t for t, _ in self.clicks) == TOGGLE_SEQUENCE and \
       (self.clicks[-1][1] - self.clicks[0][1]) * DT_CTRL <= TOGGLE_WINDOW:
      self.clicks.clear()
      return True
    return False
