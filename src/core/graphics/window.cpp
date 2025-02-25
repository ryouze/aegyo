/**
 * @file window.cpp
 */

#include <memory>  // for std::unique_ptr, std::make_unique,

#include <SFML/Graphics.hpp>
#include <fmt/core.h>

#include "settings/screen.hpp"
#include "version.hpp"
#include "window.hpp"
#if defined(_WIN32)
#include "platform/windows.hpp"
#endif

namespace core::graphics::window {

namespace {

/**
 * @brief Private helper function to get improved context settings for the SFML window.
 *
 * This overwrites the default context settings to improve the rendering quality by enabling anti-aliasing.
 *
 * @param antialiasing Anti-aliasing level (default: 8).
 *
 * @return Improved context settings.
 *
 * @note According to the SFML documentation, "if any of these settings is not supported by the graphics card, SFML tries to find the closest valid match. For example, if 4x anti-aliasing is too high, it tries 2x and then falls back to 0."
 */
[[nodiscard]] sf::ContextSettings get_improved_context_settings(const unsigned int antialiasing = 8)
{
    sf::ContextSettings settings;
    settings.antiAliasingLevel = antialiasing;
    return settings;
}

}  // namespace

std::unique_ptr<sf::RenderWindow> create_window()
{
    // Create a new SFML window
    std::unique_ptr<sf::RenderWindow> window = std::make_unique<sf::RenderWindow>(
        sf::VideoMode({core::graphics::settings::screen::WIDTH, core::graphics::settings::screen::HEIGHT}),
        fmt::format("aegyo ({})", PROJECT_VERSION),
        sf::Style::Titlebar | sf::Style::Close,  // No resize, no fullscreen (too much work to handle, especially on macOS)
        sf::State::Windowed,
        get_improved_context_settings());  // Enable anti-aliasing

    // Enable V-Sync to limit the frame rate to the refresh rate of the monitor
    window->setVerticalSyncEnabled(true);

    // Disable key repeat, as we only want one key press to register
    window->setKeyRepeatEnabled(false);

#if defined(_WIN32)
    // Set window titlebar icon (Windows-only)
    // macOS doesn't have titlebar icons, GNU/Linux is DE-dependent
    if (const auto e = core::graphics::platform::windows::set_titlebar_icon(*window); e.has_value()) {
        fmt::print(stderr, "Warning: {}\n", *e);
    }
#endif

    // Return the created window as a unique pointer
    return window;
}

}  // namespace core::graphics::window
