/**
 * @file colors.hpp
 *
 * @brief Colors inspired by the macOS dark theme.
 */

#pragma once

#include <SFML/Graphics/Color.hpp>

namespace core::settings::colors {

// Define basic colors used throughout
constexpr sf::Color background(30, 30, 30);
constexpr sf::Color text(240, 240, 240);
constexpr sf::Color enabled_toggle(100, 200, 100);
constexpr sf::Color disabled_toggle(200, 100, 100);
constexpr sf::Color default_button(80, 80, 80);
constexpr sf::Color hover_button(100, 100, 100);
constexpr sf::Color correct_answer(30, 130, 30);
constexpr sf::Color incorrect_answer(130, 30, 30);
constexpr sf::Color selected_wrong_answer(200, 120, 0);
constexpr sf::Color question_circle(50, 50, 50);

}  // namespace core::settings::colors
