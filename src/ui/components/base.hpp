/**
 * @file base.hpp
 *
 * @brief Base classes for UI components.
 */

#pragma once

#include <SFML/Graphics.hpp>

#include "core/assets/font.hpp"

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
    explicit TextInCircle(const float radius);

    /**
     * @brief Draw the circle and text to the window.
     *
     * @param window Window to draw to.
     */
    void draw(sf::RenderWindow &window) const;

  protected:
    /**
     * @brief Centered circle shape.
     */
    sf::CircleShape circle_;

    /**
     * @brief Text object.
     */
    core::assets::font::Text text_;
};

}  // namespace ui::components::base
