/**
 * @file string.hpp
 *
 * @brief Manipulate strings of text.
 */

#pragma once

#include <string>  // for std::string

#include <SFML/Graphics.hpp>

namespace core::string {

/**
 * @brief Class that provides a generic text object with the embedded font already set.
 *
 * Each constructor delegates to the main constructor.
 */
class Text : public sf::Text {
  public:
    /**
     * @brief Construct a new Text object.
     */
    Text();

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

}  // namespace core::string
