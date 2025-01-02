/**
 * @file text.cpp
 */

#include <SFML/Graphics.hpp>

#include "text.hpp"

namespace core::text {

void set_integer_position(sf::Text &text,
                          const float x,
                          const float y)
{
    text.setPosition(static_cast<int>(x), static_cast<int>(y));
}

}  // namespace core::text
