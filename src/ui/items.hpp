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
    explicit Percentage(const float top_left_offset = 10.f)  // Offset from the top-left corner
        : text_(core::assets::font::get_embedded_font()),
          correct_answers_(0),
          total_answers_(0)
    {
        this->text_.setCharacterSize(18);
        this->text_.setFillColor(core::settings::colors::text);
        const sf::Vector2f pos = sf::Vector2f(core::settings::screen::TOP_LEFT.x + top_left_offset,
                                              core::settings::screen::TOP_LEFT.y + top_left_offset);
        set_integer_position(this->text_, pos);
        this->update_score_display();
    }

    void add_correct_answer()
    {
        ++this->correct_answers_;
        ++this->total_answers_;
        this->update_score_display();
    }

    void add_incorrect_answer()
    {
        ++this->total_answers_;
        this->update_score_display();
    }

    void reset()
    {
        this->correct_answers_ = 0;
        this->total_answers_ = 0;
        this->update_score_display();
    }

    void draw(sf::RenderWindow &window) const
    {
        window.draw(this->text_);
    }

  private:
    sf::Text text_;
    std::size_t correct_answers_ = 0;
    std::size_t total_answers_ = 0;

    void update_score_display()
    {
        float percentage = 0.f;  // If no answers yet, default to 0%
        if (this->total_answers_ > 0) {
            percentage = (static_cast<float>(this->correct_answers_) / static_cast<float>(this->total_answers_)) * 100.f;
        }

        this->text_.setString(core::string::to_sfml_string(  // Unicode fix
            fmt::format("게임 점수: {:.1f}%", percentage)));
    }
};

}  // namespace ui::items
