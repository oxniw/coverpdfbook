// ==============================================================================
// File: .//Description-of-file/path_utils.md
// Overview: Implements path resolution, relative path conversion, and Windows
//           Desktop shell folder discovery.
// ==============================================================================

#include "path_utils.h" // Include path utilities header
#include <windows.h> // Include core Windows API
#include <shlobj.h> // Include Windows Shell folder APIs
#include <shlwapi.h> // Include Shell lightweight utility APIs
#include <algorithm> // Include standard algorithms

std::wstring getDesktopDirectoryPath() { // Define getDesktopDirectoryPath implementation
    wchar_t desktopPath[MAX_PATH]; // Allocate buffer for desktop folder path
    if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_DESKTOPDIRECTORY, NULL, SHGFP_TYPE_CURRENT, desktopPath))) { // Query desktop directory from shell
        return normalizePath(desktopPath); // Return normalized desktop path
    } // Close if statement
    return L"."; // Fallback to current working directory if shell query fails
} // Close getDesktopDirectoryPath function

std::wstring normalizePath(const std::wstring& path) { // Define normalizePath implementation
    if (path.empty()) return L""; // Return empty string if input is empty
    std::wstring result = path; // Copy input path string
    for (auto& ch : result) { // Iterate through each character in path
        if (ch == L'/') ch = L'\\'; // Convert forward slash to Windows backslash
    } // Close character loop
    while (result.length() > 3 && result.back() == L'\\') { // Check for trailing backslash on non-root paths
        result.pop_back(); // Remove trailing backslash
    } // Close while loop
    return result; // Return normalized path string
} // Close normalizePath function

std::wstring getParentDirectory(const std::wstring& path) { // Define getParentDirectory implementation
    std::wstring norm = normalizePath(path); // Normalize input path
    if (isRootDirectory(norm)) return L""; // Return empty string if already at filesystem root
    size_t lastSlash = norm.find_last_of(L"\\"); // Find index of last backslash separator
    if (lastSlash == std::wstring::npos) return L""; // Return empty if no separator found
    if (lastSlash == 2 && norm[1] == L':') { // Check if parent is a drive root like C:\
        return norm.substr(0, 3); // Return drive root with trailing slash
    } // Close if statement
    return norm.substr(0, lastSlash); // Return substring up to last separator
} // Close getParentDirectory function

std::wstring toRelativePath(const std::wstring& basePath, const std::wstring& targetPath) { // Define toRelativePath implementation
    wchar_t relPath[MAX_PATH] = { 0 }; // Allocate buffer for relative path
    std::wstring normBase = normalizePath(basePath); // Normalize base directory path
    std::wstring normTarget = normalizePath(targetPath); // Normalize target path
    if (PathRelativePathToW(relPath, normBase.c_str(), FILE_ATTRIBUTE_DIRECTORY, normTarget.c_str(), FILE_ATTRIBUTE_NORMAL)) { // Calculate relative path using shell API
        return std::wstring(relPath); // Return calculated relative path string
    } // Close if statement
    return normTarget; // Fallback to normalized target path if calculation fails
} // Close toRelativePath function

std::wstring resolvePath(const std::wstring& basePath, const std::wstring& subPath) { // Define resolvePath implementation
    wchar_t combined[MAX_PATH] = { 0 }; // Allocate buffer for combined path
    std::wstring normBase = normalizePath(basePath); // Normalize base directory path
    if (PathCombineW(combined, normBase.c_str(), subPath.c_str())) { // Combine base and sub-path
        wchar_t canonical[MAX_PATH] = { 0 }; // Allocate buffer for canonicalized path
        if (PathCanonicalizeW(canonical, combined)) { // Canonicalize combined path
            return normalizePath(canonical); // Return normalized canonical path
        } // Close if statement
        return normalizePath(combined); // Return normalized combined path
    } // Close if statement
    return subPath; // Fallback to subPath if combine fails
} // Close resolvePath function

bool isRootDirectory(const std::wstring& path) { // Define isRootDirectory implementation
    std::wstring norm = normalizePath(path); // Normalize path before inspection
    if (norm.length() == 3 && norm[1] == L':' && norm[2] == L'\\') return true; // Check drive root e.g. C:\
    if (norm.length() == 2 && norm[1] == L':') return true; // Check drive letter e.g. C:
    if (norm == L"\\" || norm == L"/") return true; // Check lone separator root
    return false; // Return false for non-root paths
} // Close isRootDirectory function
