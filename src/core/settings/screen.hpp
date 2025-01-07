/**
 * @file screen.hpp
 *
 * @brief Default constexpr screen settings.
 */

#pragma once

#include <SFML/System/Vector2.hpp>

namespace core::settings::screen {

/**
 * @brief Default window width in pixels.
 */
constexpr unsigned int WIDTH = 800u;

/**
 * @brief Default window height in pixels.
 */
constexpr unsigned int HEIGHT = 600u;

/**
 * @brief Top-left corner of the screen.
 */
constexpr sf::Vector2f TOP_LEFT = {0.0f, 0.0f};

// /**
//  * @brief Top-center of the screen.
//  */
// constexpr sf::Vector2f TOP_CENTER = {WIDTH / 2.0f, 0.0f};

// /**
//  * @brief Top-right corner of the screen.
//  */
// constexpr sf::Vector2f TOP_RIGHT = {WIDTH, 0.0f};

// /**
//  * @brief Center-left of the screen.
//  */
// constexpr sf::Vector2f CENTER_LEFT = {0.0f, HEIGHT / 2.0f};

/**
 * @brief Center of the screen.
 */
constexpr sf::Vector2f CENTER = {WIDTH / 2.0f, HEIGHT / 2.0f};

// /**
//  * @brief Center-right of the screen.
//  */
// constexpr sf::Vector2f CENTER_RIGHT = {WIDTH, HEIGHT / 2.0f};

// /**
//  * @brief Bottom-left corner of the screen.
//  */
// constexpr sf::Vector2f BOTTOM_LEFT = {0.0f, HEIGHT};

// /**
//  * @brief Bottom-center of the screen.
//  */
// constexpr sf::Vector2f BOTTOM_CENTER = {WIDTH / 2.0f, HEIGHT};

// /**
//  * @brief Bottom-right corner of the screen.
//  */
// constexpr sf::Vector2f BOTTOM_RIGHT = {WIDTH, HEIGHT};

}  // namespace core::settings::screen
