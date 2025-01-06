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
};

}  // namespace core::string
