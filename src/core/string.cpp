/**
 * @file string.cpp
 */

#include <string>  // for std::string

#include <SFML/Graphics.hpp>

#include "string.hpp"

namespace core::string {

sf::String to_sfml_string(const std::string &utf8_str)
{
    return sf::String::fromUtf8(utf8_str.cbegin(), utf8_str.cend());
}

}  // namespace core::string
