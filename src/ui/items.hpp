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
        : text_(core::assets::font::get_embedded_font()),
          correct_answers_(0),
          total_answers_(0)
    {
        this->text_.setCharacterSize(18);
        this->text_.setFillColor(core::settings::colors::text);
        set_integer_position(this->text_, {10.f, 10.f});
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

    /**
     * @brief Calculate the percentage of correct answers.
     *
     * @return Percentage of correct answers between 0.0 and 100.0 inclusive (e.g., 75.0).
     */
    [[nodiscard]] float calculate_percentage()
    {
        float percentage = 0.f;
        if (this->total_answers_ > 0) {
            percentage = (static_cast<float>(this->correct_answers_) / static_cast<float>(this->total_answers_)) * 100.f;
        }
        fmt::print("[DEBUG] Percentage: {:.1f}%\n", percentage);
        return percentage;
    }

    void update_score_display()
    {
        this->text_.setString(core::string::to_sfml_string(  // Unicode
            fmt::format("게임 점수: {:.1f}%", this->calculate_percentage())));
        fmt::print("[DEBUG] Updated score display to: {}\n", this->text_.getString().toAnsiString());
    }
};

}  // namespace ui::items
