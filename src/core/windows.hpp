/**
 * @file windows.hpp
 *
 * @brief Input/output functions.
 */

#pragma once

#if defined(_WIN32)

#include <optional>  // for std::optional
#include <string>    // for std::string

#include <SFML/Window.hpp>

namespace core::windows {

/**
 * @brief Setup UTF-8 input/output on Windows. Do nothing on other platforms.
 *
 * @return Error message if the setup failed, "std::nullopt" otherwise.
 */
[[nodiscard]] std::optional<std::string> setup_utf8_console();

/**
 * @brief Setup the titlebar icon on Windows using the embedded icon data (must be embedded by CMake).
 *
 * @param window Window to set the titlebar icon for.
 *
 * @return Error message if the setup failed, "std::nullopt" otherwise.
 */
std::optional<std::string> setup_titlebar_icon(const sf::Window &window);

}  // namespace core::windows

#endif
