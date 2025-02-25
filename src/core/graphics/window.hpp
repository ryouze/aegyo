/**
 * @file window.hpp
 *
 * @brief Create SFML window with sane defaults.
 */

#pragma once

#include <memory>  // for std::unique_ptr

#include <SFML/Graphics/RenderWindow.hpp>

namespace core::graphics::window {

/**
 * @brief Create a new SFML window with sane defaults.
 *
 * This includes:
 * - Setting dimensions to 800x600, non-resizable
 * - Setting the title to "aegyo (v$PROJECT_VERSION)"
 * - Enabling 8x anti-aliasing
 * - Enabling V-Sync
 * - Disabling key repeat
 * - Setting the window icon (Windows-only)
 *
 * @return Pointer to the created window. The ownership is transferred to the caller. The caller is also responsible for adding an event loop and rendering logic.
 */
[[nodiscard]] std::unique_ptr<sf::RenderWindow> create();

}  // namespace core::graphics::window
