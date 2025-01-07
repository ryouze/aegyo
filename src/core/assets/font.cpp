/**
 * @file font.cpp
 */

#include <optional>   // for std::optional
#include <stdexcept>  // for std::runtime_error
#include <string>     // for std::string
#include <utility>    // for std::move

#include <SFML/Graphics.hpp>

#include "font.hpp"
#include "raw/NanumGothic.hpp"

namespace core::assets::font {

namespace {

/**
 * @brief Private helper function to load the embedded font data.
 *
 * @return Embedded font data.
 *
 * @throws std::runtime_error If the font data cannot be loaded.
 */
[[nodiscard]] const sf::Font &get_embedded_font()
{
    static std::optional<sf::Font> font_opt;

    if (!font_opt.has_value()) {
        sf::Font font;
        if (!font.openFromMemory(NanumGothic::data, NanumGothic::size)) {
            throw std::runtime_error("Failed to load embedded font data");
        }
        font_opt = std::move(font);
    }

    return *font_opt;
}

/**
 * @brief Private helper to convert a UTF-8 string to an SFML string.
 *
 * @param utf8_str String to convert (e.g., "Dzień dobry").
 *
 * @return SFML string (e.g., "Dzień dobry").
 */
[[nodiscard]] sf::String to_sfml_string(const std::string &utf8_str)
{
    return sf::String::fromUtf8(utf8_str.cbegin(), utf8_str.cend());
}

}  // namespace

Text::Text()
    : sf::Text(core::assets::font::get_embedded_font(), "", 30) {}

void Text::setString(const std::string &utf8_str)
{
    sf::Text::setString(to_sfml_string(utf8_str));
}

void Text::setPosition(const sf::Vector2f &position)
{
    sf::Text::setPosition({static_cast<float>(static_cast<int>(position.x)),
                           static_cast<float>(static_cast<int>(position.y))});
}

void Text::resetOrigin()
{
    const sf::FloatRect tb = this->getLocalBounds();
    sf::Text::setOrigin({tb.position.x + tb.size.x / 2.f,
                         tb.position.y + tb.size.y / 2.f});
}

}  // namespace core::assets::font
