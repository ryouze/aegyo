/**
 * @file antialiasing.cpp
 */

#include <SFML/Window/ContextSettings.hpp>

#include "antialiasing.hpp"

namespace core::settings::antialiasing {

sf::ContextSettings get_improved_context_settings(const unsigned int antialiasing)
{
    sf::ContextSettings settings;
    settings.antiAliasingLevel = antialiasing;
    return settings;
}

}  // namespace core::settings::antialiasing
