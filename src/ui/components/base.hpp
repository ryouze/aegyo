/**
 * @file base.hpp
 *
 * @brief Base classes for UI components.
 */

#pragma once

#include <string>  // for std::string

#include <SFML/Graphics.hpp>

#include "core/settings/colors.hpp"
#include "core/string.hpp"

namespace ui::components::base {

/**
 * @brief Class that represents a circle item with a text label inside it.
 *
 * On construction, the class initializes a shape and text pair, then sets the origin to the center of the circle instead of the top-left corner.
 */
class TextInCircle {
  public:
    /**
     * @brief Construct a new TextInCircle object.
     *
     * @param radius Radius of the circle (e.g., "80.f").
     */
    explicit TextInCircle(const float radius)
        : circle_(radius, 100)  // Use 100 points for a smooth circle
    {
        // Circle
        this->circle_.setOrigin({radius, radius});  // Set the origin to the center of the circle

        // Text
        this->text_.setFillColor(core::settings::colors::text);
    }

    /**
     * @brief Draw the circle and text to the window.
     *
     * @param window Window to draw to.
     */
    void draw(sf::RenderWindow &window) const
    {
        window.draw(this->circle_);
        window.draw(this->text_);
    }

  protected:
    sf::CircleShape circle_;
    core::string::Text text_;
};

}  // namespace ui::components::base
