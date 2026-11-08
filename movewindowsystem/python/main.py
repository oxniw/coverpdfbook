# ==============================================================================
# File: .//Description-of-file/main_py.md
# Overview: Application entry point for Python CLI orchestrating window discovery, repositioning, and hotkey daemon.
# ==============================================================================

import argparse # Import argparse for parsing command line arguments
import logging # Import logging module for structured diagnostic logging
import sys # Import sys for system exit codes
from config import AlignmentMode, DEFAULT_OFFSET_X, DEFAULT_OFFSET_Y, MOVE_WINDOW_SUCCESS # Import config constants
from move_window_api import MoveWindowNativeAPI # Import native API wrapper
from window_finder import SelectorStrategy, WindowResolver # Import window finder components
from hotkey_listener import HotkeyDaemon # Import hotkey daemon listener

logging.basicConfig(level=logging.INFO, format="%(asctime)s [%(levelname)s] %(message)s") # Configure standard logging format
logger = logging.getLogger("move_window_cli") # Instantiate CLI logger

def parse_args() -> argparse.Namespace: # Define command line argument parsing function
    parser = argparse.ArgumentParser(description="Move input window to mouse cursor location.") # Instantiate argument parser
    group = parser.add_mutually_exclusive_group() # Create mutually exclusive group for target selection
    group.add_argument("--title", type=str, help="Target window title substring") # Add title flag
    group.add_argument("--process", type=str, help="Target process executable name (e.g. notepad.exe)") # Add process flag
    group.add_argument("--hwnd", type=str, help="Target window handle (hex e.g. 0x1A2B or decimal)") # Add raw HWND flag
    group.add_argument("--active", action="store_true", help="Target currently active foreground window") # Add active window flag

    parser.add_argument("--align", choices=["center", "topleft", "custom"], default="center", help="Window anchor alignment") # Add alignment argument
    parser.add_argument("--offset-x", type=int, default=DEFAULT_OFFSET_X, help="Custom horizontal pixel offset") # Add offset X argument
    parser.add_argument("--offset-y", type=int, default=DEFAULT_OFFSET_Y, help="Custom vertical pixel offset") # Add offset Y argument
    parser.add_argument("--no-clamp", action="store_true", help="Disable screen boundary clamping") # Add no-clamp flag
    parser.add_argument("--daemon", action="store_true", help="Run background hotkey daemon (Ctrl+Alt+M)") # Add daemon mode flag
    parser.add_argument("--dll-path", type=str, default=None, help="Explicit path to move_window.dll") # Add custom DLL path argument

    return parser.parse_args() # Parse and return arguments namespace

def resolve_target_strategy(args: argparse.Namespace) -> tuple[SelectorStrategy, str | None]: # Map CLI args to strategy
    if args.title: # Check if title argument was provided
        return (SelectorStrategy.TITLE_SUBSTRING, args.title) # Return title strategy
    elif args.process: # Check if process argument was provided
        return (SelectorStrategy.PROCESS_NAME, args.process) # Return process strategy
    elif args.hwnd: # Check if raw handle argument was provided
        return (SelectorStrategy.RAW_HWND, args.hwnd) # Return raw handle strategy
    return (SelectorStrategy.ACTIVE_FOREGROUND, None) # Default to active foreground strategy

def map_alignment_mode(align_str: str) -> AlignmentMode: # Convert string alignment to enum
    if align_str == "topleft": # Check for topleft option
        return AlignmentMode.TOP_LEFT # Return TOP_LEFT enum
    elif align_str == "custom": # Check for custom option
        return AlignmentMode.CUSTOM_OFFSET # Return CUSTOM_OFFSET enum
    return AlignmentMode.CENTER # Default to CENTER enum

def execute_reposition(api: MoveWindowNativeAPI, resolver: WindowResolver, strategy: SelectorStrategy, query: str | None, align: AlignmentMode, off_x: int, off_y: int, clamp: bool) -> int: # Execute move
    target_hwnd = resolver.resolve(strategy, query) # Resolve target window HWND
    if not target_hwnd: # Validate handle was discovered
        logger.error(f"Target window could not be found with strategy: {strategy.value} (query: {query})") # Log error
        return -1 # Return failure code

    bounds = api.get_window_bounds(target_hwnd) # Query current window bounds
    cursor = api.get_cursor_position() # Query current cursor coordinates
    logger.info(f"Target HWND: {hex(target_hwnd)}, Current Bounds: {bounds}, Cursor: {cursor}") # Log target and cursor status

    status = api.move_window_to_cursor(target_hwnd, mode=align, offset_x=off_x, offset_y=off_y, clamp=clamp) # Move window to cursor
    if status == MOVE_WINDOW_SUCCESS: # Check for success return code
        logger.info(f"Window {hex(target_hwnd)} successfully moved to cursor location.") # Log success
    else: # Error branch
        logger.error(f"Failed to move window {hex(target_hwnd)}. Error code: {status}") # Log error code
    return status # Return operation status code

def main() -> None: # Main orchestration function
    args = parse_args() # Parse incoming command-line options
    api = MoveWindowNativeAPI(dll_path=args.dll_path) # Initialize native library interface
    resolver = WindowResolver(api) # Initialize window resolver
    strategy, query = resolve_target_strategy(args) # Determine selection strategy and query
    align_mode = map_alignment_mode(args.align) # Determine alignment mode enum
    clamp = not args.no_clamp # Determine clamping flag

    if args.daemon: # Check if daemon mode requested
        logger.info("Starting background daemon. Press Ctrl+Alt+M to move target window to cursor.") # Log daemon startup
        daemon = HotkeyDaemon( # Construct hotkey daemon instance
            callback=lambda: execute_reposition(api, resolver, strategy, query, align_mode, args.offset_x, args.offset_y, clamp) # Action callback
        ) # End HotkeyDaemon constructor
        daemon.start_blocking() # Run synchronous message pump
        sys.exit(0) # Exit cleanly after daemon shutdown

    status = execute_reposition(api, resolver, strategy, query, align_mode, args.offset_x, args.offset_y, clamp) # Perform one-shot reposition
    sys.exit(0 if status == MOVE_WINDOW_SUCCESS else 1) # Exit with appropriate status code

if __name__ == "__main__": # Script execution guard
    main() # Call main function
