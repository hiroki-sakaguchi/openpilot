#!/usr/bin/env python3
from collections import deque
from dataclasses import dataclass
from time import monotonic

from common.params import Params
from opendbc.car import structs

ButtonType = structs.CarState.ButtonEvent.Type


@dataclass
class CruiseInput:
  input_type: ButtonType
  timestamp: float


class ExperimentalModeToggle:
  """Handles cruise control input sequence detection for toggling experimental mode."""
  
  # Input sequence: DOWN -> UP -> DOWN within 3 seconds (works with both lever and button types)
  SEQUENCE_TIMEOUT = 3.0  # seconds
  EXPECTED_SEQUENCE = [ButtonType.decelCruise, ButtonType.accelCruise, ButtonType.decelCruise]
  
  def __init__(self):
    self.params = Params()
    self.input_history = deque(maxlen=3)
    self.experimental_mode_confirmed = self._is_experimental_mode_confirmed()
  
  def _is_experimental_mode_confirmed(self) -> bool:
    """Check if experimental mode has been confirmed by the user."""
    # Check if experimental mode has been acknowledged
    return self.params.get_bool("ExperimentalModeConfirmed")
  
  def update(self, car_state: structs.CarState) -> bool:
    """
    Update cruise control input sequence detection and return True if experimental mode should be toggled.
    
    Args:
      car_state: Current car state containing input events and cruise state
      
    Returns:
      True if experimental mode should be toggled, False otherwise
    """
    # Only process if cruise control is enabled
    if not car_state.cruiseState.enabled:
      self.input_history.clear()
      return False
    
    # Process cruise control input events
    for event in car_state.buttonEvents:
      if event.pressed and event.type in [ButtonType.accelCruise, ButtonType.decelCruise]:
        self.input_history.append(CruiseInput(event.type, monotonic()))
    
    # Check if we have the complete sequence
    if len(self.input_history) < 3:
      return False
    
    # Check if the sequence matches and is within timeout
    current_time = monotonic()
    sequence_duration = current_time - self.input_history[0].timestamp
    
    if sequence_duration > self.SEQUENCE_TIMEOUT:
      # Sequence too old, clear oldest entry
      self.input_history.popleft()
      return False
    
    # Check if the sequence matches
    sequence_matches = all(
      cruise_input.input_type == expected_type 
      for cruise_input, expected_type in zip(self.input_history, self.EXPECTED_SEQUENCE)
    )
    
    if sequence_matches:
      # Clear history to prevent repeated toggles
      self.input_history.clear()
      
      # Toggle experimental mode
      current_mode = self.params.get_bool("ExperimentalMode")
      
      # If turning on experimental mode and not yet confirmed, don't toggle
      # The UI will handle showing the confirmation dialog
      if not current_mode and not self.experimental_mode_confirmed:
        # Set a flag to indicate that toggle was requested
        self.params.put_bool("ExperimentalModeToggleRequested", True)
        return False
      
      # Toggle the mode
      self.params.put_bool("ExperimentalMode", not current_mode)
      return True
    
    return False
  
  def check_confirmation_completed(self):
    """Check if experimental mode confirmation has been completed and clear the request flag."""
    if self.params.get_bool("ExperimentalModeToggleRequested"):
      self.experimental_mode_confirmed = self._is_experimental_mode_confirmed()
      if self.experimental_mode_confirmed:
        # Confirmation completed, clear the request flag
        self.params.put_bool("ExperimentalModeToggleRequested", False)
        # Now actually enable experimental mode
        self.params.put_bool("ExperimentalMode", True)