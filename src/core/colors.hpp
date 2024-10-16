/**
 * @file colors.hpp
 *
 * @brief Defines color constants used throughout the application.
 */

#pragma once

#include <SFML/Graphics/Color.hpp>

namespace core::colors {

// Background color
inline const sf::Color Background = sf::Color(30, 30, 30);

// Text color
inline const sf::Color Text = sf::Color(240, 240, 240);

// Button colors
inline const sf::Color EnabledColor = sf::Color(100, 200, 100);   // Green for enabled
inline const sf::Color DisabledColor = sf::Color(200, 100, 100);  // Red for disabled
inline const sf::Color HoverColor = sf::Color(150, 150, 150);     // Gray for hover

// Answer button colors
inline const sf::Color DefaultButton = sf::Color(80, 80, 80);
inline const sf::Color HoverButton = sf::Color(100, 100, 100);
inline const sf::Color CorrectAnswer = sf::Color(0, 200, 0);
inline const sf::Color IncorrectAnswer = sf::Color(200, 0, 0);
inline const sf::Color SelectedWrongAnswer = sf::Color(255, 165, 0);  // Orange

// Question circle color
inline const sf::Color QuestionCircle = sf::Color(50, 50, 50);

}  // namespace core::colors
