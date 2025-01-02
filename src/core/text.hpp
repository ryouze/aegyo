/**
 * @file text.hpp
 *
 * @brief SFML Text utilities.
 */

#pragma once

#include <SFML/Graphics.hpp>

namespace core::text {

/**
 * @brief Set the position of an SFML text object using integer coordinates.
 *
 * The provided x and y coordinates are cast to integers before setting the position.
 *
 * @param text SFML text object to set the position for.
 * @param x X-coordinate (e.g., "10.0").
 * @param y Y-coordinate (e.g., "123.4").
 */
void set_integer_position(sf::Text &text,
                          const float x,
                          const float y);

/**
 * @brief Set the position of an SFML text object using integer coordinates.
 *
 * The provided coordinate is cast to an integer before setting the position.
 *
 * @param text SFML text object to set the position for.
 * @param position X and Y coordinates (e.g., {10.0, 123.4}).
 */
void set_integer_position(sf::Text &text,
                          const sf::Vector2f &position);

}  // namespace core::text
