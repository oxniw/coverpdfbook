@echo off
setlocal

echo ========================================================
echo Building Radial Menu System (C / C++ / Win32 / GDI+)
echo ========================================================

set CXX=g++
set CXXFLAGS=-O2 -Wall -Wextra -std=c++17 -Iinclude -DBUILDING_RADIAL_DLL
set LIBS=-static -lgdi32 -lgdiplus -luser32

echo Compiling radial_menu.dll ...
%CXX% %CXXFLAGS% -shared -o radial_menu.dll ^
    src/math_geometry.c ^
    src/menu_state.c ^
    src/animation_manager.c ^
    src/render_overlay.cpp ^
    src/overlay_window.cpp ^
    src/mouse_hook.cpp ^
    src/radial_controller.cpp ^
    src/radial_api.cpp ^
    %LIBS% -Wl,--out-implib,libradial_menu.a

if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] DLL compilation failed!
    exit /b %ERRORLEVEL%
)

echo Compiling standalone radial_menu_cpp.exe ...
%CXX% -O2 -Wall -std=c++17 -Iinclude -o radial_menu_cpp.exe ^
    main.cpp ^
    -L. -lradial_menu %LIBS%

if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Executable compilation failed!
    exit /b %ERRORLEVEL%
)

echo [SUCCESS] Build complete: radial_menu.dll and radial_menu_cpp.exe generated.
endlocal
