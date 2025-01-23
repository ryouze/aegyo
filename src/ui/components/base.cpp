/**
 * @file base.cpp
 */

#include <string>  // for std::string

#include <SFML/Graphics.hpp>

#include "base.hpp"
#include "core/settings/colors.hpp"

namespace ui::components::base {

namespace {

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

Text::Text(const sf::Font &font)
    : sf::Text(font, "", 30) {}

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

TextInCircle::TextInCircle(const sf::Font &font,
                           const float radius)
    : circle_(radius, 100),  // Use 100 points for a smooth circle
      text_(font)
{
    // Circle
    this->circle_.setOrigin({radius, radius});  // Set the origin to the center of the circle

    // Text
    this->text_.setFillColor(core::settings::colors::text::normal);
}

void TextInCircle::draw(sf::RenderWindow &window) const
{
    window.draw(this->circle_);
    window.draw(this->text_);
}

}  // namespace ui::components::base
