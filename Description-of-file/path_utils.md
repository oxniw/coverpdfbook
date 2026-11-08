# Description: path_utils

## Overview
Filesystem path resolution and navigation utility module providing Windows Desktop discovery, path normalization, relative path calculation, and hierarchical directory traversal.

## Exports & Functions
- `getDesktopDirectoryPath() -> std::wstring`
  - **Description**: Queries the Windows Shell for the current user's desktop folder path (`CSIDL_DESKTOPDIRECTORY`).
  - **Parameters**: None.
  - **Returns**: Normalized path string to Desktop, or `.` as fallback.

- `normalizePath(const std::wstring& path) -> std::wstring`
  - **Description**: Converts all forward slashes to backslashes and trims redundant trailing slashes.
  - **Parameters**:
    - `path`: Input path string.
  - **Returns**: Normalized path string.

- `getParentDirectory(const std::wstring& path) -> std::wstring`
  - **Description**: Traces path backwards to locate the parent directory; correctly preserves root drive prefixes.
  - **Parameters**:
    - `path`: Current directory path.
  - **Returns**: Parent directory path, or empty string if at filesystem root.

- `toRelativePath(const std::wstring& basePath, const std::wstring& targetPath) -> std::wstring`
  - **Description**: Calculates a concise relative path between a base path (e.g. Desktop) and target path.
  - **Parameters**:
    - `basePath`: Starting directory path.
    - `targetPath`: Destination file or directory path.
  - **Returns**: Relative path string.

- `resolvePath(const std::wstring& basePath, const std::wstring& subPath) -> std::wstring`
  - **Description**: Combines and canonicalizes a base directory and relative sub-path into a clean absolute path.
  - **Parameters**:
    - `basePath`: Base directory path.
    - `subPath`: Relative or absolute target path.
  - **Returns**: Resolved canonical path.

- `isRootDirectory(const std::wstring& path) -> bool`
  - **Description**: Determines whether the provided path represents a filesystem drive root (e.g. `C:\`).
  - **Parameters**:
    - `path`: Directory path to inspect.
  - **Returns**: `true` if root, `false` otherwise.

## Dependencies & Data Flow
- **Imports**: `<windows.h>`, `<shlobj.h>`, `<shlwapi.h>`, `<algorithm>`.
- **Invoked By**: Called from `main.cpp`, `scan_pdf_directory.cpp`, `handle_user_input.cpp`, and `create_control_panel.cpp`.
- **Data Flow**: Path inputs -> Shell canonicalization -> Normalized paths for navigation and display.
