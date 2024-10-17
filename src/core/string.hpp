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
 * @brief Convert a UTF-8 string to an SFML string.
 *
 * @param utf8_str String to convert (e.g., "Dzień dobry").
 *
 * @return SFML string (e.g., "Dzień dobry").
 */
[[nodiscard]] sf::String to_sfml_string(const std::string &utf8_str);

}  // namespace core::string
