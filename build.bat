@echo off
echo Compiling PDF Book Cover Overlay with Radial Menu and Move Window Integration...
if not exist "radial_menu.dll" (
    copy "redialmenu\radial_menu.dll" "radial_menu.dll" >nul
)
if not exist "move_window.dll" (
    copy "movewindowsystem\build\move_window.dll" "move_window.dll" >nul
)
g++ -std=c++17 -O2 main.cpp scan_pdf_directory.cpp extract_pdf_cover.cpp calculate_grid_layout.cpp render_overlay_canvas.cpp create_control_panel.cpp handle_user_input.cpp radial_menu_integration.cpp page_animation_manager.cpp path_utils.cpp move_window_integration.cpp preload_manager.cpp -L./redialmenu -lradial_menu -L./movewindowsystem/build -lmove_window -o coverpdfbook.exe -lgdiplus -lgdi32 -lole32 -lshell32 -loleaut32 -luuid -lshlwapi -mwindows
if %ERRORLEVEL% EQU 0 (
    echo [SUCCESS] Built coverpdfbook.exe with Radial Menu and Move Window System successfully!
) else (
    echo [ERROR] Build failed!
)
