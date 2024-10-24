/**
 * @file io.cpp
 */

#if defined(_WIN32)

#define WIN32_LEAN_AND_MEAN  // Exclude rarely-used stuff from Windows headers
// #include <cstdio>            // for freopen_s
#include <locale>            // for setlocale, LC_ALL
#include <optional>          // for std::optional, std::nullopt
#include <string>            // for std::string
#include <windows.h>         // for CP_UTF8, SetConsoleCP, SetConsoleOutputCP, FreeConsole, AttachConsole, GetLastError

#include <fmt/core.h>

#include "io.hpp"

namespace core::io {

// std::optional<std::string> attach_to_console()
// {
//     if (AttachConsole(ATTACH_PARENT_PROCESS)) {
//         FILE *fp;
//         // Redirect stdout, stderr, and stdin to the attached console
//         freopen_s(&fp, "CONOUT$", "w", stdout);
//         freopen_s(&fp, "CONOUT$", "w", stderr);
//         freopen_s(&fp, "CONIN$", "r", stdin);
//         return std::nullopt;  // Success
//     }
//     else {
//         // If there's no parent process console, FreeConsole to avoid issues
//         FreeConsole();
//         return "No console available to attach to";
//     }
// }

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

}  // namespace core::io

#endif
