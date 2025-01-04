/**
 * @file text.cpp
 */

#include <SFML/Graphics.hpp>

#include "text.hpp"

namespace core::text {

// void set_integer_position(sf::Text &text,
//                           const float x,
//                           const float y)
// {
//     // Cast to integer, then back to float
//     text.setPosition({static_cast<float>(static_cast<int>(x)),
//                       static_cast<float>(static_cast<int>(y))});
// }

void set_integer_position(sf::Text &text,
                          const sf::Vector2f &position)
{
    // Cast to integer, then back to float
    text.setPosition({static_cast<float>(static_cast<int>(position.x)),
                      static_cast<float>(static_cast<int>(position.y))});
}

}  // namespace core::text
