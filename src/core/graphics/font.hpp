/**
 * @file font.hpp
 *
 * @brief Embedded font data loader.
 */

#pragma once

#include <memory>  // for std::unique_ptr

#include <SFML/Graphics/Font.hpp>

namespace core::graphics::font {

/**
 * @brief Load the embedded font data into an "sf::Font" object.
 *
 * @return Pointer to the loaded font. The ownership is transferred to the caller.
 *
 * @throws std::runtime_error if the font cannot be loaded.
 */
[[nodiscard]] std::unique_ptr<sf::Font> load();

}  // namespace core::graphics::font
