/**
 * @file percentage.hpp
 *
 * @brief Percentage of correct answers.
 */

#pragma once

#include <cstddef>  // for std::size_t
#include <string>   // for std::string

#include <SFML/Graphics.hpp>
#include <fmt/core.h>

#include "core/assets/font.hpp"
#include "core/settings/colors.hpp"
#include "core/settings/screen.hpp"

namespace ui::percentage {

class Percentage {
  public:
    explicit Percentage()
        : correct_answers_(0),
          total_answers_(0)
    {
        // Appearance
        this->text_.setCharacterSize(18);
        this->text_.setFillColor(core::settings::colors::text);

        // Position
        constexpr float top_left_offset = 10.f;  // Offset from the top-left corner
        this->text_.setPosition({core::settings::screen::TOP_LEFT.x + top_left_offset,
                                 core::settings::screen::TOP_LEFT.y + top_left_offset});
        this->update_text();
    }

    void add_correct_answer()
    {
        ++this->correct_answers_;
        ++this->total_answers_;
        this->update_text();
    }

    void add_incorrect_answer()
    {
        ++this->total_answers_;
        this->update_text();
    }

    void reset()
    {
        this->correct_answers_ = 0;
        this->total_answers_ = 0;
        this->update_text();
    }

    /**
     * @brief Draw the text to the window.
     *
     * @param window Window to draw to.
     */
    void draw(sf::RenderWindow &window) const
    {
        window.draw(this->text_);
    }

  private:
    void update_text()
    {
        float percent = 100.f;  // If no answers yet, default to 100%
        if (this->total_answers_ > 0) {
            percent = (static_cast<float>(this->correct_answers_) /
                       static_cast<float>(this->total_answers_)) *
                      100.f;
        }
        this->text_.setString(fmt::format("게임 점수: {:.1f}%", percent));
    }

    core::assets::font::Text text_;
    std::size_t correct_answers_;
    std::size_t total_answers_;
};

}  // namespace ui::percentage
