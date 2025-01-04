/**
 * @file font.cpp
 */

#include <optional>   // for std::optional
#include <stdexcept>  // for std::runtime_error
#include <utility>    // for std::move

#include <SFML/Graphics.hpp>

#include "font.hpp"
#include "raw/NanumGothic.hpp"

namespace core::assets::font {

const sf::Font &get_embedded_font()
{
    static std::optional<sf::Font> font_opt;

    if (!font_opt.has_value()) {
        sf::Font font;
        if (!font.loadFromMemory(NanumGothic::data, NanumGothic::size)) {
            throw std::runtime_error("Failed to load embedded font data");
        }
        font_opt = std::move(font);
    }

    return *font_opt;
}

}  // namespace core::assets::font
