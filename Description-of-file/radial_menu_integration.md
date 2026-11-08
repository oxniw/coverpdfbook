# Description: radial_menu_integration

## Overview
Connects the 6-sector gesture-based radial (pie) menu system from `redialmenu/` to the PDF bookshelf application. When the user holds the Right Mouse Button and drags on the screen, a translucent radial menu appears with 6 sectors controlling page navigation, closing the program, toggling settings at cursor, toggling click-through, and toggling cover visibility.

## Exports & Functions
- `initRadialMenuIntegration(HWND hwndOverlay) -> bool`
  - **Description**: Initializes the radial menu engine via `radial_init()`, sets the sector count to 6, assigns custom action labels (Sector 1: "Close Program", Sector 2: "Next Page >>", Sector 3: "Settings", Sector 4: "Click-Through", Sector 5: "<< Prev Page", Sector 6: "Hide/Show"), registers `onRadialSectorAction` callback, and installs the low-level mouse hook (`WH_MOUSE_LL`).
  - **Parameters**:
    - `hwndOverlay`: Handle to the main overlay window for receiving action commands.
  - **Returns**: `true` if initialization and hook installation succeeded, `false` otherwise.
- `shutdownRadialMenuIntegration() -> void`
  - **Description**: Unhooks the low-level mouse hook and cleans up radial menu resources.
  - **Parameters**: None.
  - **Returns**: None.
- `discoverPdfFolders() -> std::vector<std::wstring>`
  - **Description**: Enumerates all subfolders in the workspace (e.g. `pdf`, `tgat`, `tpat`, `other`, `asd`), ignoring internal system directories.
  - **Parameters**: None.
  - **Returns**: Vector of folder names containing PDF content.

## Dependencies & Data Flow
- **Imports**: `radial_menu_integration.h`, `config.h`, `redialmenu/include/radial_api.h`, `<windows.h>`, `<algorithm>`, `<vector>`, `<string>`.
- **Invoked By**: Called from `main.cpp` (`wWinMain` on startup and shutdown).
- **Data Flow**: Right-mouse drag gesture anywhere on desktop -> Radial menu evaluates release sector (Sector 2 = Close Program) -> `onRadialSectorAction` posts `WM_COMMAND` -> Main window executes action and cleanly shuts down.
