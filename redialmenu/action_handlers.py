# ==============================================================================
# File: .//Description-of-file/action_handlers.md
# Overview: OS automation handlers executing productivity shortcuts for sectors.
# ==============================================================================

import ctypes
from ctypes import wintypes
import time
from config import RadialSector, SECTOR_LABELS
from logger import setup_logger

logger = setup_logger("ActionHandlers")

# Windows Virtual-Key Codes
VK_CONTROL = 0x11
VK_C = 0x43
VK_V = 0x56
VK_Z = 0x5A
VK_Y = 0x59
KEYEVENTF_KEYUP = 0x0002
INPUT_KEYBOARD = 1

class KEYBDINPUT(ctypes.Structure):
    _fields_ = [
        ("wVk", wintypes.WORD),
        ("wScan", wintypes.WORD),
        ("dwFlags", wintypes.DWORD),
        ("time", wintypes.DWORD),
        ("dwExtraInfo", ctypes.c_ulonglong)
    ]

class INPUT(ctypes.Structure):
    class _INPUT_UNION(ctypes.Union):
        _fields_ = [("ki", KEYBDINPUT)]
    _anonymous_ = ("u",)
    _fields_ = [
        ("type", wintypes.DWORD),
        ("u", _INPUT_UNION)
    ]

def _send_key_event(vk_code: int, flags: int) -> None:
    inp = INPUT(type=INPUT_KEYBOARD)
    inp.ki.wVk = vk_code
    inp.ki.wScan = 0
    inp.ki.dwFlags = flags
    inp.ki.time = 0
    inp.ki.dwExtraInfo = 0
    ctypes.windll.user32.SendInput(1, ctypes.byref(inp), ctypes.sizeof(INPUT))

def send_hotkey(modifier_vk: int, key_vk: int) -> None:
    """
    Simulates pressing modifier + key combo and releasing them.
    """
    _send_key_event(modifier_vk, 0)
    time.sleep(0.01)
    _send_key_event(key_vk, 0)
    time.sleep(0.01)
    _send_key_event(key_vk, KEYEVENTF_KEYUP)
    time.sleep(0.01)
    _send_key_event(modifier_vk, KEYEVENTF_KEYUP)

def handle_function1() -> None:
    """
    Function 1 (Top-Right): Copy shortcut (Ctrl+C).
    """
    logger.info("Executing Function 1: Copy [Ctrl+C]")
    send_hotkey(VK_CONTROL, VK_C)

def handle_function2() -> None:
    """
    Function 2 (Bottom-Right): Paste shortcut (Ctrl+V).
    """
    logger.info("Executing Function 2: Paste [Ctrl+V]")
    send_hotkey(VK_CONTROL, VK_V)

def handle_function3() -> None:
    """
    Function 3 (Bottom-Left): Undo shortcut (Ctrl+Z).
    """
    logger.info("Executing Function 3: Undo [Ctrl+Z]")
    send_hotkey(VK_CONTROL, VK_Z)

def handle_function4() -> None:
    """
    Function 4 (Top-Left): Redo shortcut (Ctrl+Y).
    """
    logger.info("Executing Function 4: Redo [Ctrl+Y]")
    send_hotkey(VK_CONTROL, VK_Y)

ACTION_REGISTRY = {
    RadialSector.FUNCTION1: handle_function1,
    RadialSector.FUNCTION2: handle_function2,
    RadialSector.FUNCTION3: handle_function3,
    RadialSector.FUNCTION4: handle_function4,
}

def register_action_handler(sector_id: int, handler: callable) -> None:
    """
    Registers custom function handler for a sector index.
    """
    ACTION_REGISTRY[sector_id] = handler

def dispatch_action(sector_id: int) -> None:
    """
    Dispatches the action corresponding to the triggered sector.
    """
    if sector_id == 0:
        logger.info("Gesture cancelled (Nothing).")
        return

    label = SECTOR_LABELS.get(sector_id, f"Function {sector_id}")
    logger.info(f"Sector activated: Index {sector_id} ({label})")

    handler = ACTION_REGISTRY.get(sector_id)
    if handler:
        handler()
    else:
        logger.warning(f"No action registered for sector index {sector_id}")
