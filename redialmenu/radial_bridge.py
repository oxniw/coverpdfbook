# ==============================================================================
# File: .//Description-of-file/radial_bridge.md
# Overview: Ctypes wrapper bridging Python application logic with radial_menu.dll.
# ==============================================================================

import ctypes
import os
import sys
from typing import Callable
from config import DLL_NAME
from logger import setup_logger

logger = setup_logger("RadialBridge")

# C Callback type: void (*RadialActionCallback)(int32_t sector, void* user_data)
ACTION_CALLBACK_TYPE = ctypes.CFUNCTYPE(None, ctypes.c_int32, ctypes.c_void_p)

class RadialMenuBridge:
    """
    Python wrapper managing the native C/C++ radial_menu.dll engine.
    """

    def __init__(self, dll_path: str = None) -> None:
        if dll_path is None:
            base_dir = os.path.dirname(os.path.abspath(__file__))
            dll_path = os.path.join(base_dir, DLL_NAME)

        if not os.path.exists(dll_path):
            error_msg = f"Native library not found at: {dll_path}. Run build.bat first."
            logger.error(error_msg)
            raise FileNotFoundError(error_msg)

        logger.info(f"Loading native library from: {dll_path}")
        self._dll = ctypes.CDLL(dll_path)
        self._callback_ref = None

        self._bind_functions()

    def _bind_functions(self) -> None:
        # radial_init
        self._dll.radial_init.argtypes = []
        self._dll.radial_init.restype = ctypes.c_int

        # radial_set_callback
        self._dll.radial_set_callback.argtypes = [ACTION_CALLBACK_TYPE, ctypes.c_void_p]
        self._dll.radial_set_callback.restype = None

        # radial_start_hook
        self._dll.radial_start_hook.argtypes = []
        self._dll.radial_start_hook.restype = ctypes.c_int

        # radial_stop_hook
        self._dll.radial_stop_hook.argtypes = []
        self._dll.radial_stop_hook.restype = None

        # radial_poll_events
        self._dll.radial_poll_events.argtypes = []
        self._dll.radial_poll_events.restype = ctypes.c_int

        # radial_run_loop
        self._dll.radial_run_loop.argtypes = []
        self._dll.radial_run_loop.restype = None

        # radial_shutdown
        self._dll.radial_shutdown.argtypes = []
        self._dll.radial_shutdown.restype = None

        # radial_set_sector_count
        self._dll.radial_set_sector_count.argtypes = [ctypes.c_int]
        self._dll.radial_set_sector_count.restype = None

        # radial_get_sector_count
        self._dll.radial_get_sector_count.argtypes = []
        self._dll.radial_get_sector_count.restype = ctypes.c_int

        # radial_set_sector_label
        self._dll.radial_set_sector_label.argtypes = [ctypes.c_int, ctypes.c_wchar_p]
        self._dll.radial_set_sector_label.restype = None

    def initialize(self) -> bool:
        """
        Initializes GDI+ overlay window and geometry engines.
        """
        success = bool(self._dll.radial_init())
        if success:
            logger.info("Radial menu native engine initialized successfully.")
        else:
            logger.error("Failed to initialize radial menu native engine.")
        return success

    def set_callback(self, py_callback: Callable[[int], None]) -> None:
        """
        Registers Python handler for confirmed slice actions.
        """
        def _native_wrapper(sector: int, user_data: int) -> None:
            try:
                py_callback(sector)
            except Exception as e:
                logger.error(f"Error executing Python callback: {e}", exc_info=True)

        self._callback_ref = ACTION_CALLBACK_TYPE(_native_wrapper)
        self._dll.radial_set_callback(self._callback_ref, None)
        logger.info("Registered action callback with native engine.")

    def start_hook(self) -> bool:
        """
        Installs low-level mouse hook.
        """
        result = bool(self._dll.radial_start_hook())
        if result:
            logger.info("Mouse hook installed. Ready for right-click gestures.")
        else:
            logger.error("Failed to install low-level mouse hook.")
        return result

    def stop_hook(self) -> None:
        """
        Uninstalls low-level mouse hook.
        """
        self._dll.radial_stop_hook()
        logger.info("Mouse hook uninstalled.")

    def poll_events(self) -> int:
        """
        Polls single Windows message (non-blocking).
        """
        return self._dll.radial_poll_events()

    def run_loop(self) -> None:
        """
        Runs blocking Windows message pump.
        """
        logger.info("Entering radial menu message loop.")
        self._dll.radial_run_loop()

    def set_sector_count(self, count: int) -> None:
        """
        Sets dynamic outer sector count N (e.g. 2, 3, 4, 6, 8, up to 16).
        """
        self._dll.radial_set_sector_count(int(count))
        logger.info(f"Configured outer sector count N = {count}")

    def get_sector_count(self) -> int:
        """
        Returns currently configured outer sector count N.
        """
        return int(self._dll.radial_get_sector_count())

    def set_sector_label(self, sector_index: int, label: str) -> None:
        """
        Sets custom display label for sector (1 to N).
        """
        self._dll.radial_set_sector_label(int(sector_index), str(label))

    def shutdown(self) -> None:
        """
        Stops hook, cleans up memory DC and windows.
        """
        logger.info("Shutting down native engine...")
        self._dll.radial_shutdown()
