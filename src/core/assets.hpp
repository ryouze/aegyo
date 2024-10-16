/**
 * @file assets.hpp
 *
 * @brief Load embedded font data.
 */

#pragma once

#include <SFML/Graphics.hpp>

namespace core::assets {

/**
 * @brief Load the embedded font data.
 *
 * @return Embedded font data.
 *
 * @throws std::runtime_error If the font data cannot be loaded.
 */
[[nodiscard]] const sf::Font &load_font();

}  // namespace core::assets
