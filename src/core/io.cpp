/**
 * @file io.cpp
 */

#if defined(_WIN32)

#define WIN32_LEAN_AND_MEAN  // Exclude rarely-used stuff from Windows headers

#define IDI_ICON1 101  // Icon resource ID, must match the one in CMakeLists.txt

#include <locale>     // for setlocale, LC_ALL
#include <optional>   // for std::optional, std::nullopt
#include <string>     // for std::string
#include <windows.h>  // for SetConsoleCP, SetConsoleOutputCP, GetLastError, HWND, HICON, LoadIcon, GetModuleHandle, MAKEINTRESOURCE, SendMessage, WM_SETICON, ICON_BIG, ICON_SMALL, LPARAM

#include <SFML/Window.hpp>
#include <fmt/core.h>

#include "io.hpp"

namespace core::io {

std::optional<std::string> setup_utf8_console()
{
    if (!SetConsoleCP(CP_UTF8) || !SetConsoleOutputCP(CP_UTF8)) {
        return fmt::format("Failed to set UTF-8 code page: {}", GetLastError());
    }

    if (!setlocale(LC_ALL, ".UTF8")) {
        return "Failed to set UTF-8 locale";
    }
    return std::nullopt;
}

std::optional<std::string> setup_titlebar_icon(const sf::Window &window)
{
    // Get the native window handle from the SFML window
    const HWND hwnd = window.getSystemHandle();
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

}  // namespace core::io

#endif
