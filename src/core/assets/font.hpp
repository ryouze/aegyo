/**
 * @file font.hpp
 *
 * @brief Embedded font data.
 */

#pragma once

#include <SFML/Graphics/Font.hpp>

namespace core::assets::font {

/**
 * @brief Load the embedded font data.
 *
 * @return Embedded font data.
 *
 * @throws std::runtime_error If the font data cannot be loaded.
 */
[[nodiscard]] const sf::Font &get_embedded_font();

}  // namespace core::assets::font
