# ==============================================================================
# File: .//Description-of-file/window_finder_py.md
# Overview: High-level Python window resolver matching targets by title, process, HWND, or active focus.
# ==============================================================================

from enum import Enum # Import Enum for strategy types
from typing import Optional # Import Optional type hint
from move_window_api import MoveWindowNativeAPI # Import native API wrapper

class SelectorStrategy(Enum): # Define selector strategy enumeration
    ACTIVE_FOREGROUND = "active" # Match current foreground window
    TITLE_SUBSTRING = "title" # Match window by title substring
    PROCESS_NAME = "process" # Match window by process executable name
    RAW_HWND = "hwnd" # Match window by direct numeric HWND

class WindowResolver: # Define window resolver class
    def __init__(self, api: MoveWindowNativeAPI): # Initialize with API instance
        self._api = api # Store API instance reference

    def resolve(self, strategy: SelectorStrategy, query: Optional[str] = None) -> Optional[int]: # Resolve window handle
        if strategy == SelectorStrategy.ACTIVE_FOREGROUND: # Active foreground branch
            return self._api.get_foreground_window() # Return active window handle
        elif strategy == SelectorStrategy.TITLE_SUBSTRING and query: # Title substring branch
            return self._api.find_by_title(query) # Return handle found by title
        elif strategy == SelectorStrategy.PROCESS_NAME and query: # Process name branch
            return self._api.find_by_process(query) # Return handle found by process
        elif strategy == SelectorStrategy.RAW_HWND and query: # Raw handle branch
            try: # Begin exception block
                val = int(query, 16) if query.lower().startswith("0x") else int(query) # Parse hex or decimal integer
                return val # Return parsed integer handle
            except ValueError: # Catch invalid integer format
                return None # Return None on parse failure
        return None # Return None if unresolved
