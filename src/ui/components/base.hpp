/**
 * @file base.hpp
 *
 * @brief Base classes for UI components.
 */

#pragma once

#include <string>  // for std::string

#include <SFML/Graphics.hpp>

namespace ui::components::base {

/**
 * @brief Class that provides a generic text object with sane defaults and UTF-8 support.
 */
class Text : public sf::Text {
  public:
    /**
     * @brief Construct a new Text object.
     *
     * @param font Font to use for the text object.
     */
    Text(const sf::Font &font);

    /**
     * @brief Set the string's text.
     *
     * To ensure unicode support, the string is converted to an SFML string before setting.
     *
     * @param utf8_str String to set (e.g., "こんにちは").
     */
    void setString(const std::string &utf8_str);

    /**
     * @brief Set the position of the text object using integer coordinates.
     *
     * The provided coordinate is cast to an integer before setting the position.
     *
     * @param position X and Y coordinates (e.g., {10.0, 123.4}).
     */
    void setPosition(const sf::Vector2f &position);

    /**
     * @brief Reset the origin of the text object.
     *
     * The origin is set to the center of the object.
     */
    void resetOrigin();
};

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
     * @param font Font to use for the text object.
     * @param radius Radius of the circle (e.g., "80.f").
     */
    explicit TextInCircle(const sf::Font &font,
                          const float radius);

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
    Text text_;
};

}  // namespace ui::components::base
