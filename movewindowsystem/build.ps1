# ==============================================================================
# File: .//Description-of-file/build_ps1.md
# Overview: One-click PowerShell compilation script configuring and building the C DLL and C++ CLI executable.
# ==============================================================================

param( # Declare script parameters
    [string]$BuildType = "Release" # Default build configuration type
) # End param block

$ErrorActionPreference = "Stop" # Terminate script immediately upon error

$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path # Get current script directory
Set-Location $scriptDir # Set working directory to script location

Write-Host "Configuring CMake project (BuildType: $BuildType)..." -ForegroundColor Cyan # Output configuration notification
if (-not (Test-Path "build")) { # Check if build directory exists
    New-Item -ItemType Directory -Path "build" | Out-Null # Create build directory
} # End check

cmake -B build -S . -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=$BuildType # Run CMake generation using MinGW Makefiles

Write-Host "Compiling move_window.dll and move_window_cli.exe..." -ForegroundColor Cyan # Output build status notification
cmake --build build --config $BuildType # Build project binaries

if (Test-Path "build/move_window.dll") { # Verify DLL output exists
    Copy-Item -Path "build/move_window.dll" -Destination "python/move_window.dll" -Force # Ensure Python has latest DLL copy
} elseif (Test-Path "build/libmove_window.dll") { # Check if GCC prefixed library exists
    Copy-Item -Path "build/libmove_window.dll" -Destination "python/move_window.dll" -Force # Copy as move_window.dll
    Copy-Item -Path "build/libmove_window.dll" -Destination "python/libmove_window.dll" -Force # Keep prefixed copy as well
} # End copy check

Write-Host "Build complete! Artifacts are located in build/ and python/." -ForegroundColor Green # Output success notification
