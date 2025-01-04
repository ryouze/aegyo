/**
 * @file items.hpp
 *
 * @brief SFML components and items.
 */

#pragma once

#include <cstddef>  // for std::size_t

#include <SFML/Graphics.hpp>
#include <fmt/core.h>

#include "core/assets/font.hpp"
#include "core/settings/colors.hpp"
#include "core/settings/screen.hpp"
#include "core/string.hpp"

namespace ui::items {

/**
 * @brief Set the position of an SFML text object using integer coordinates.
 *
 * The provided coordinate is cast to an integer before setting the position.
 *
 * @param text SFML text object to set the position for.
 * @param position X and Y coordinates (e.g., {10.0, 123.4}).
 */
void set_integer_position(sf::Text &text,
                          const sf::Vector2f &position)
{
    // Cast to integer, then back to float
    text.setPosition({static_cast<float>(static_cast<int>(position.x)),
                      static_cast<float>(static_cast<int>(position.y))});
}

class Percentage {
  public:
    explicit Percentage()
        : text_(core::assets::font::get_embedded_font())
    {
        this->text_.setCharacterSize(18);
        this->text_.setFillColor(core::settings::colors::text);
        set_integer_position(this->text_, {10.f, 10.f});
    }

    void set_number(const float percentage)
    {
        this->text_.setString(core::string::to_sfml_string(fmt::format("게임 점수: {:.1f}%", percentage)));
    }

    void draw(sf::RenderWindow &window) const
    {
        window.draw(this->text_);
    }

  private:
    sf::Text text_;
};

}  // namespace ui::items
