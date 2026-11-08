# Radial Menu Gesture System

A high-performance, modular gesture-based radial (pie) menu system implemented across **C, C++, and Python** according to `/future-proof-architecture`, `/write-script-separated-file-function`, and `/prevent-print`.

Holding down the **Right Mouse Button** and dragging away from the click position brings up a transparent radial wheel overlay centered at the initial click coordinates. Releasing over a sector triggers its action, while releasing in the center circle cancels. Standard quick right-clicks pass through to Windows context menus without interference.

---

## Sector Layout

The layout directly mirrors the quadrant donut design:

```text
               | 12 o'clock
        function4  |  function1
     (Top-Left)   |  (Top-Right)
                  |
  9 o'clock ------+------ 3 o'clock
            /  Nothing  \
           |   (Cancel)  |
            \           /
  ----------------+-----------------
                  |
        function3  |  function2
    (Bottom-Left) |  (Bottom-Right)
               | 6 o'clock
```

| Sector | Position | Default Action | Implementation |
| :--- | :--- | :--- | :--- |
| **Center** | Inner Circle ($\le 45\text{px}$) | **Nothing** (Cancel) | Dismisses overlay with no side-effects |
| **function1** | Top-Right Quadrant ($dx \ge 0, dy < 0$) | **Copy** (`Ctrl + C`) | Windows `SendInput` keystroke simulation |
| **function2** | Bottom-Right Quadrant ($dx \ge 0, dy \ge 0$) | **Paste** (`Ctrl + V`) | Windows `SendInput` keystroke simulation |
| **function3** | Bottom-Left Quadrant ($dx < 0, dy \ge 0$) | **Undo** (`Ctrl + Z`) | Windows `SendInput` keystroke simulation |
| **function4** | Top-Left Quadrant ($dx < 0, dy < 0$) | **Redo** (`Ctrl + Y`) | Windows `SendInput` keystroke simulation |

---

## Architectural Separation & Seams

In accordance with `/future-proof-architecture`:
- **Stable Core (Pure Domain)**: `math_geometry.c` and `menu_state.c` contain zero Windows OS or GUI dependencies. They can be compiled and unit-tested on any platform.
- **Explicit Seam (C-ABI)**: `radial_api.h` and `radial_api.cpp` export a stable C interface (`radial_menu.dll`) insulating consumers from internal changes.
- **Adapters**:
  - `render_overlay.cpp`: High-DPI GDI+ vector graphics renderer into 32-bit ARGB DIB.
  - `overlay_window.cpp`: Win32 layered transparent window (`WS_EX_LAYERED | WS_EX_NOACTIVATE`) that never steals focus.
  - `mouse_hook.cpp`: Low-level mouse hook (`WH_MOUSE_LL`) with synthetic right-click pass-through replay.
  - `radial_bridge.py`: Python `ctypes` bridge with standard library only (zero external pip packages required).
  - `action_handlers.py`: Customizable productivity handlers.

---

## Project Structure

```text
rightclickdrag/
├── Description-of-file/           # Documentation for every source file
│   ├── action_handlers.md
│   ├── config_h.md
│   ├── config_py.md
│   ├── logger_py.md
│   ├── main_cpp.md
│   ├── main_py.md
│   ├── math_geometry.md
│   ├── menu_state.md
│   ├── mouse_hook.md
│   ├── overlay_window.md
│   ├── radial_api_cpp.md
│   ├── radial_api_h.md
│   ├── radial_bridge.md
│   ├── radial_controller.md
│   ├── radial_types.md
│   ├── render_overlay.md
│   └── test_geometry.md
├── include/
│   ├── config.h                  # C/C++ visual metrics and colors
│   ├── radial_api.h              # Exported C-ABI header
│   └── radial_types.h            # Sector enums, structs, callbacks
├── src/
│   ├── animation_manager.c/.h    # Pure parametric easing, spring pop-in & hover lerp
│   ├── math_geometry.c/.h        # Pure Euclidean distance and sector resolution
│   ├── menu_state.c/.h           # Pure state machine tracking gestures
│   ├── mouse_hook.cpp/.h         # Low-level WH_MOUSE_LL hook and pass-through
│   ├── overlay_window.cpp/.h     # Win32 transparent layered window
│   ├── radial_api.cpp            # Exported DLL functions implementation
│   ├── radial_controller.cpp/.h  # Controller orchestrating state and UI
│   └── render_overlay.cpp/.h     # GDI+ pie rendering & typography
├── tests/
│   └── test_geometry.c           # Pure domain unit tests
├── action_handlers.py            # Python shortcut actions (Copy, Paste, Undo, Redo)
├── build.bat                     # Compilation script for GCC / MinGW
├── config.py                     # Python configuration constants
├── logger.py                     # Leveled structured logger (prevent-print)
├── main.cpp                      # Standalone native C++ application
├── main.py                       # Main Python entry point
├── radial_bridge.py              # ctypes DLL wrapper
└── README.md                     # Project documentation
```

---

## How to Build & Run

### 1. Build Native C/C++ DLL and Standalone Executable
Ensure `g++` (MinGW-w64) is in your PATH, then run:
```cmd
build.bat
```
This generates:
- `radial_menu.dll`: Self-contained C-ABI dynamic library.
- `radial_menu_cpp.exe`: Standalone native C++ executable.

### 2. Run with Python Bridge
```cmd
python main.py
```
- Open any text editor, web browser, or file explorer.
- **Hold Right Mouse Button and drag** in any direction: the radial wheel appears.
- Release inside a quadrant to trigger the shortcut.
- Release in the center circle ("Nothing") to cancel.
- A quick tap on the right mouse button opens the normal context menu as usual.

### 3. Run Standalone C++ Executable
```cmd
radial_menu_cpp.exe
```

### 4. Run Unit Tests
```cmd
gcc -O2 -I. -Iinclude tests\test_geometry.c src\math_geometry.c src\menu_state.c -o tests\test_geometry.exe -lm
.\tests\test_geometry.exe
```
