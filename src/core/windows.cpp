/**
 * @file windows.cpp
 */

#if defined(_WIN32)

#define WIN32_LEAN_AND_MEAN  // Exclude rarely-used stuff from Windows headers

#define IDI_ICON1 101  // Icon resource ID, must match the generated "icon.rc" in CMakeLists.txt
// Typically, you’d define this ID in a "resource.h" file and include it in both "icon.rc" and this file
// However, I want to avoid any platform-specific files whenever possible

#include <optional>  // for std::optional, std::nullopt
#include <string>    // for std::string

#include <windows.h>  // for HWND, HICON, LoadIcon, GetModuleHandle, MAKEINTRESOURCE, GetLastError, SendMessage, WM_SETICON, ICON_BIG, ICON_SMALL, LPARAM

#include <SFML/Window.hpp>
#include <fmt/core.h>

#include "windows.hpp"

namespace core::windows {

std::optional<std::string> setup_titlebar_icon(const sf::Window &window)
{
    // Get the native window handle from the SFML window
    const HWND hwnd = window.getNativeHandle();
    if (hwnd == NULL) {
        return "Window handle is null";
    }

    // Load the embedded icon resource
    const HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
    if (hIcon == NULL) {
        return fmt::format("Failed to load icon resource: {}", GetLastError());
    }

    // Set the big and small icons for the window
    SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
    SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);

    return std::nullopt;
}

}  // namespace core::windows

#endif
