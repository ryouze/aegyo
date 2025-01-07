/**
 * @file colors.hpp
 *
 * @brief Colors inspired by the macOS dark theme.
 */

#pragma once

#include <SFML/Graphics/Color.hpp>

namespace core::settings::colors {

/**
 * @brief Window background colors.
 */
namespace background {
constexpr sf::Color normal(30, 30, 30);  // Generic background color
}  // namespace background

/**
 * @brief Text colors.
 */
namespace text {
constexpr sf::Color normal(240, 240, 240);  // Generic text color
}  // namespace text

/**
 * @brief Question and answer circle colors.
 */
namespace circle {
constexpr sf::Color normal(80, 80, 80);       // Generic circle color (large question circle)
constexpr sf::Color darker(50, 50, 50);       // Darker circle color (small answer circles)
constexpr sf::Color hover(100, 100, 100);     // Hovered circle color (small answer circles)
constexpr sf::Color disabled(200, 100, 100);  // Disabled circle color (for invalid answers in both question and answer circles)
// Correct and incorrect colors
constexpr sf::Color correct(30, 130, 30);               // Green correct answer color (small answer circles)
constexpr sf::Color incorrect_selected(200, 120, 0);    // Orange incorrect answer color (small answer circles)
constexpr sf::Color incorrect_unselected(130, 30, 30);  // Red incorrect answer color (small answer circles)
}  // namespace circle

/**
 * @brief Vocabulary category toggle colors in the top right corner.
 */
namespace category {
constexpr sf::Color enabled(100, 200, 100);   // Enabled category color
constexpr sf::Color disabled(200, 100, 100);  // Disabled category color
}  // namespace category

}  // namespace core::settings::colors
