# ==============================================================================
# File: .//Description-of-file/config_py.md
# Overview: Application configuration, sector definitions, and shortcut mappings.
# ==============================================================================

from enum import IntEnum

class RadialSector(IntEnum):
    """
    Radial slice indices matching the quadrant layout:
    - NOTHING (0): Center dead-zone / Cancel
    - FUNCTION1 (1): Top-Right quadrant
    - FUNCTION2 (2): Bottom-Right quadrant
    - FUNCTION3 (3): Bottom-Left quadrant
    - FUNCTION4 (4): Top-Left quadrant
    """
    NOTHING = 0
    FUNCTION1 = 1
    FUNCTION2 = 2
    FUNCTION3 = 3
    FUNCTION4 = 4

# DLL binary name
DLL_NAME = "radial_menu.dll"

# Visual Geometry Configuration
INNER_RADIUS_PX = 45.0
OUTER_RADIUS_PX = 150.0
WINDOW_SIZE_PX = 400
DRAG_THRESHOLD_PX = 18.0

# Dynamic Action Configuration (N functions)
# You can freely add, remove, or reorder actions here (from 2 up to 16 functions)!
ACTIONS_CONFIG = [
    {"id": 1, "label": "Copy", "description": "Copy [Ctrl+C]"},
    {"id": 2, "label": "Paste", "description": "Paste [Ctrl+V]"},
    {"id": 3, "label": "Undo", "description": "Undo [Ctrl+Z]"},
    {"id": 4, "label": "Redo", "description": "Redo [Ctrl+Y]"},
]

# Human-readable labels for the sectors
SECTOR_LABELS = {
    RadialSector.NOTHING: "Nothing (Cancel)",
    RadialSector.FUNCTION1: "Copy (Ctrl+C)",
    RadialSector.FUNCTION2: "Paste (Ctrl+V)",
    RadialSector.FUNCTION3: "Undo (Ctrl+Z)",
    RadialSector.FUNCTION4: "Redo / Enter"
}
