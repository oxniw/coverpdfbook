# Description: config.h

## Overview
Central configuration header defining shared application structures, default geometric constants, control identifier enumerations, grid alignment anchors, and global hotkey bindings.

## Exports & Functions
- `enum GridAlignment`
  - **Description**: Defines available grid anchor positions on the desktop screen: `ALIGN_TOP_LEFT`, `ALIGN_TOP_CENTER`, `ALIGN_TOP_RIGHT`, `ALIGN_MIDDLE_LEFT`, `ALIGN_CENTER`, `ALIGN_MIDDLE_RIGHT`, `ALIGN_BOTTOM_LEFT`, `ALIGN_BOTTOM_CENTER`, `ALIGN_BOTTOM_RIGHT`.
- `getAlignmentName(GridAlignment align) -> const wchar_t*`
  - **Description**: Returns clean string representation of an alignment mode for UI display.
- `struct PdfItem`
  - **Description**: Encapsulates metadata for a discovered PDF file, its absolute filesystem path, user-friendly display title, cached GDI+ cover bitmap, and screen bounding box.
- `struct AppSettings`
  - **Description**: Holds runtime user-modifiable state including grid rows ($M$), grid columns ($N$), cover width/height sizing, spacing, alpha transparency level, click-through mode, current page, and grid start alignment anchor.
- `struct GridLayoutResult`
  - **Description**: Stores calculated pagination and bounding metrics including total items, items per page, total pages, start/end indices for the active page, and total visible items.
- `enum ControlIDs`
  - **Description**: Unique identifiers for UI buttons and text displays in the floating control panel including alignment navigation buttons `IDC_BTN_ALIGN_PREV` and `IDC_BTN_ALIGN_NEXT`.
- `enum HotkeyIDs`
  - **Description**: IDs for registered global hotkeys (`Ctrl+Alt+T`, `Ctrl+Alt+H`, `Ctrl+Alt+R`).

## Dependencies & Data Flow
- **Imports**: `<windows.h>`, `<wtypes.h>`, `<propidl.h>`, `<gdiplus.h>`, `<string>`, `<vector>`.
- **Invoked By**: Consumed across all translation units.
- **Data Flow**: Supplies data structures, defaults, and type definitions consumed and updated by both UI inputs and rendering passes.
