/**
 * @file antialiasing.hpp
 *
 * @brief Default antialiasing level.
 */

#pragma once

#include <SFML/Window/ContextSettings.hpp>

namespace core::settings::antialiasing {

/**
 * @brief Get improved context settings for the SFML window.
 *
 * This overwrites the default context settings to improve the rendering quality by enabling anti-aliasing.
 *
 * @param antialiasing Anti-aliasing level (default: 8).
 *
 * @return Improved context settings.
 *
 * @note According to the SFML documentation, "if any of these settings is not supported by the graphics card, SFML tries to find the closest valid match. For example, if 4x anti-aliasing is too high, it tries 2x and then falls back to 0."
 */
[[nodiscard]] sf::ContextSettings get_improved_context_settings(const unsigned int antialiasing = 8);

}  // namespace core::settings::antialiasing
