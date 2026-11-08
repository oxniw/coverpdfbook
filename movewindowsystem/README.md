# Move Window System & Move-Window-API

A modular, future-proof window manipulation system that repositions any target window to the current mouse cursor location. Implemented across `.h`, `.c`, `.cpp`, and `.python` with a high-performance C ABI dynamic library (`move_window.dll`), a modern C++ CLI executable (`move_window_cli.exe`), and an idiomatic Python API (`move_window_api.py`).

---

## Architecture Overview

```text
movewindowsystem/
├── CMakeLists.txt                         # CMake build specification
├── build.ps1                              # One-click PowerShell compilation script
├── Description-of-file/                   # Granular documentation for each source file
│   ├── build_ps1.md
│   ├── cmakelists_txt.md
│   ├── config_h.md
│   ├── config_py.md
│   ├── geometry_calc_c.md
│   ├── geometry_calc_h.md
│   ├── geometry_calc_py.md
│   ├── hotkey_listener_cpp.md
│   ├── hotkey_listener_hpp.md
│   ├── hotkey_listener_py.md
│   ├── main_cpp.md
│   ├── main_py.md
│   ├── move_window_api_c.md
│   ├── move_window_api_h.md
│   ├── move_window_api_py.md
│   ├── test_suite_py.md
│   ├── win32_window_adapter_c.md
│   ├── win32_window_adapter_h.md
│   ├── window_finder_cpp.md
│   ├── window_finder_hpp.md
│   └── window_finder_py.md
├── include/
│   ├── config.h                           # Enums, constants, return codes
│   ├── geometry_calc.h                    # Pure domain math (coordinate calculations & clamping)
│   ├── win32_window_adapter.h             # Win32 OS adapter seam (user32.dll isolation)
│   ├── move_window_api.h                  # Public C ABI library interface
│   ├── window_finder.hpp                  # C++ multi-strategy window resolver
│   └── hotkey_listener.hpp                # C++ background hotkey listener daemon
├── src/
│   ├── geometry_calc.c                    # Pure math implementation
│   ├── win32_window_adapter.c             # Win32 adapter implementation
│   ├── move_window_api.c                  # Core C ABI DLL exports implementation
│   ├── window_finder.cpp                  # C++ window finder implementation
│   ├── hotkey_listener.cpp                # C++ message pump and hotkey implementation
│   └── main.cpp                           # C++ entry point CLI
└── python/
    ├── config.py                          # Python configuration and type definitions
    ├── geometry_calc.py                   # Python domain math mirror
    ├── move_window_api.py                 # Python ctypes binding to move_window.dll
    ├── window_finder.py                   # Python strategy resolver
    ├── hotkey_listener.py                 # Python background hotkey daemon
    ├── main.py                            # Python entry point CLI
    └── test_suite.py                      # Automated unit and integration test suite
```

---

## Build Instructions

Run the one-click build script via PowerShell:
```powershell
.\build.ps1
```
This builds:
- `build/move_window.dll` (also automatically copied to `python/move_window.dll`)
- `build/move_window_cli.exe`

---

## Usage Guide

### 1. Python CLI (`python/main.py`)

#### Move Active / Focused Window to Cursor:
```powershell
python python/main.py --active
```

#### Move by Window Title Substring:
```powershell
python python/main.py --title "Notepad"
```

#### Move by Process Executable Name:
```powershell
python python/main.py --process "notepad.exe"
```

#### Move by Exact Window Handle (HWND):
```powershell
python python/main.py --hwnd 0x001A2B3C
```

#### Alignment & Clamping Options:
- `--align center` (Default): Center of window lands on cursor.
- `--align topleft`: Top-left corner lands on cursor.
- `--align custom --offset-x 30 --offset-y 30`: Place top-left at cursor + (30, 30).
- `--no-clamp`: Disable automatic screen boundary clamping.

#### Background Daemon Mode (Hotkey Trigger):
Run continuously in the background. Press `Ctrl + Alt + M` to snap the target window to wherever your cursor currently is:
```powershell
python python/main.py --process "notepad.exe" --daemon
```

---

### 2. C++ CLI Executable (`build/move_window_cli.exe`)

```powershell
# Move window by title substring
.\build\move_window_cli.exe --title "Notepad" --align center

# Move active window
.\build\move_window_cli.exe --active

# Background daemon mode (Ctrl + Alt + M)
.\build\move_window_cli.exe --process "notepad.exe" --daemon
```

---

### 3. Python API Integration (`python/move_window_api.py`)

```python
import sys
sys.path.insert(0, "python")
from move_window_api import MoveWindowNativeAPI
from config import AlignmentMode

api = MoveWindowNativeAPI()

# Find window by title and move to mouse cursor
hwnd = api.find_by_title("Notepad")
if hwnd:
    api.move_window_to_cursor(hwnd, mode=AlignmentMode.CENTER, clamp=True)
```

---

### 4. C / C++ API Integration (`include/move_window_api.h`)

Link against `move_window.dll` or `move_window.lib`:
```c
#include "move_window_api.h"

// Move window by title substring to cursor
int status = move_window_by_title("Notepad", ALIGN_CENTER, 0, 0, 1);
```

---

## Running Automated Tests

Run the test suite verifying pure geometry calculations and native dynamic library bindings:
```powershell
python python/test_suite.py
```
