// ==============================================================================
// File: .//Description-of-file/path_utils.md
// Overview: Header defining path manipulation, desktop directory resolution,
//           relative path calculations, and filesystem navigation utilities.
// ==============================================================================

#pragma once // Prevent duplicate header inclusion

#include <string> // Include standard string library
#include <vector> // Include standard vector library

// Returns the absolute path to the current user's Windows Desktop directory.
std::wstring getDesktopDirectoryPath(); // Declare getDesktopDirectoryPath function

// Normalizes path separators to standard backslashes and removes trailing slashes.
std::wstring normalizePath(const std::wstring& path); // Declare normalizePath function

// Returns the parent directory of the given path, or empty string if at root.
std::wstring getParentDirectory(const std::wstring& path); // Declare getParentDirectory function

// Calculates a relative path from basePath to targetPath.
std::wstring toRelativePath(const std::wstring& basePath, const std::wstring& targetPath); // Declare toRelativePath function

// Resolves a relative or absolute path against a base directory.
std::wstring resolvePath(const std::wstring& basePath, const std::wstring& subPath); // Declare resolvePath function

// Determines whether the specified path represents a root directory (e.g. C:\).
bool isRootDirectory(const std::wstring& path); // Declare isRootDirectory function
