/**
 * @file base.cpp
 */

#include <SFML/Graphics.hpp>

#include "base.hpp"
#include "core/settings/colors.hpp"

namespace ui::components::base {

TextInCircle::TextInCircle(const float radius)
    : circle_(radius, 100)  // Use 100 points for a smooth circle
{
    // Circle
    this->circle_.setOrigin({radius, radius});  // Set the origin to the center of the circle

    // Text
    this->text_.setFillColor(core::settings::colors::text);
}

void TextInCircle::draw(sf::RenderWindow &window) const
{
    window.draw(this->circle_);
    window.draw(this->text_);
}

}  // namespace ui::components::base
