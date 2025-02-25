/**
 * @file shapes.hpp
 *
 * @brief Base UI components for rendering text and shapes.
 */

#pragma once

#include <string>  // for std::string

#include <SFML/Graphics.hpp>

namespace core::shapes {

/**
 * @brief Class that provides a generic text object with UTF-8 support, integer positioning, and origin resetting.
 */
class Text : public sf::Text {
  public:
    using sf::Text::Text;  // Inherit all constructors from sf::Text

    /**
     * @brief Set the string's text.
     *
     * The provided UTF-8 string is converted to an SFML string before setting to ensure that unicode characters are displayed correctly.
     *
     * @param utf8_str UTF-8 string to set (e.g., "こんにちは").
     */
    void setString(const std::string &utf8_str);

    /**
     * @brief Set the position of the text object using integer coordinates.
     *
     * The provided coordinate is cast to an integer before setting the position.
     *
     * @param position X and Y coordinates (e.g., {10.0, 123.4}).
     *
     * @note The default "setPosition" method in SFML uses float coordinates, which may result in blurry text,
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
 *
 * @note This class is designed to be used as a base class for other shapes that require a text label inside them. Using it on its own is probably not very useful.
 */
class BaseCircleWithText {
  public:
    /**
     * @brief Construct a new BaseCircleWithText object.
     *
     * @param font Font to use for the text object.
     * @param radius Radius of the circle (e.g., "80.f").
     */
    explicit BaseCircleWithText(const sf::Font &font,
                                const float radius);

    /**
     * @brief Draw the circle and text to the window.
     *
     * @param window Window to draw to.
     */
    void draw(sf::RenderWindow &window) const;

  protected:  // These will be available to derived classes
    /**
     * @brief Centered circle shape.
     */
    sf::CircleShape circle_;

    /**
     * @brief Text object.
     */
    Text text_;
};

}  // namespace core::shapes
