/**
 * @file io.cpp
 */

#if defined(_WIN32)

#define WIN32_LEAN_AND_MEAN  // Exclude rarely-used stuff from Windows headers

#include <locale>     // for setlocale, LC_ALL
#include <optional>   // for std::optional, std::nullopt
#include <string>     // for std::string
#include <windows.h>  // for CP_UTF8, SetConsoleCP, SetConsoleOutputCP, GetLastError, FindResource, LoadResource, LockResource, SizeofResource, HRSRC, HGLOBAL, RT_ICON, MAKEINTRESOURCE, DWORD

#include <SFML/Graphics.hpp>
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

std::optional<std::string> setup_windows_icon(sf::RenderWindow &window)
{
    // Locate the icon in the executable resources
    const HRSRC h_resource = FindResource(nullptr, MAKEINTRESOURCE(IDI_ICON1), RT_ICON);
    if (h_resource == nullptr) {
        return "Failed to find icon resource";
    }

    // Load the resource into memory
    const HGLOBAL h_memory = LoadResource(nullptr, h_resource);
    if (h_memory == nullptr) {
        return "Failed to load icon resource";
    }

    // Access the resource data
    const DWORD icon_size = SizeofResource(nullptr, h_resource);
    const void *p_icon_data = LockResource(h_memory);
    if (p_icon_data == nullptr || icon_size == 0) {
        return "Failed to lock icon resource or resource size is zero";
    }

    // Load icon into SFML image from memory
    sf::Image icon;
    if (!icon.loadFromMemory(p_icon_data, icon_size)) {
        return "Failed to load icon from memory";
    }

    // Set the icon for the window
    window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

    return std::nullopt;
}

}  // namespace core::io

#endif
