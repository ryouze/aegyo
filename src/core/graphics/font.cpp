/**
 * @file font.cpp
 */

#include <memory>     // for std::unique_ptr, std::make_unique,
#include <stdexcept>  // for std::runtime_error

#include <SFML/Graphics/Font.hpp>

#include "assets/NanumGothic.hpp"
#include "font.hpp"

namespace core::graphics::font {

std::unique_ptr<sf::Font> load()
{
    std::unique_ptr<sf::Font> font = std::make_unique<sf::Font>();
    // Load the embedded char array into the font object
    if (!font->openFromMemory(core::graphics::assets::NanumGothic::data,
                              core::graphics::assets::NanumGothic::size)) {
        throw std::runtime_error("Failed to load embedded font data");
    }
    return font;
}  // Here, the unique_ptr is returned, transferring ownership

}  // namespace core::graphics::font
