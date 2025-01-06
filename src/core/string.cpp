/**
 * @file string.cpp
 */

#include <string>  // for std::string

#include <SFML/Graphics.hpp>

#include "assets/font.hpp"
#include "string.hpp"

namespace core::string {

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

Text::Text()
    : sf::Text(core::assets::font::get_embedded_font(), "", 30) {}

void Text::setString(const std::string &utf8_str)
{
    sf::Text::setString(to_sfml_string(utf8_str));
}

}  // namespace core::string
