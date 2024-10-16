/**
 * @file colors.hpp
 *
 * @brief SFML colors.
 */

#pragma once

#include <SFML/Graphics/Color.hpp>

namespace core::colors {

// Background color
inline const sf::Color background = sf::Color(30, 30, 30);

// Text color
inline const sf::Color text = sf::Color(240, 240, 240);

// Button colors
inline const sf::Color enabled_color = sf::Color(100, 200, 100);   // Green for enabled
inline const sf::Color disabled_color = sf::Color(200, 100, 100);  // Red for disabled
inline const sf::Color hover_color = sf::Color(150, 150, 150);     // Gray for hover

// Answer button colors
inline const sf::Color default_button = sf::Color(80, 80, 80);
inline const sf::Color hover_button = sf::Color(100, 100, 100);
inline const sf::Color correct_answer = sf::Color(0, 200, 0);
inline const sf::Color incorrect_answer = sf::Color(200, 0, 0);
inline const sf::Color selected_wrong_answer = sf::Color(255, 165, 0);  // Orange

// Question circle color
inline const sf::Color question_circle = sf::Color(50, 50, 50);

}  // namespace core::colors
