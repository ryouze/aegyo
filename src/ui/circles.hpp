/**
 * @file circles.hpp
 *
 * @brief Question and answer circles.
 */

#pragma once

#include <exception>  // for std::runtime_error
#include <string>     // for std::string

#include <SFML/Graphics.hpp>

#include "components/base.hpp"
#include "core/settings/colors.hpp"
#include "core/settings/screen.hpp"

namespace ui::circles {

class Question : public ui::components::base::TextInCircle {
  public:
    explicit Question();

    void set_invalid();

    void set_question(const std::string &latin_or_hangul);
};

enum class AnswerPosition {
    TopLeft,
    TopRight,
    BottomLeft,
    BottomRight
};

class Answer : public ui::components::base::TextInCircle {
  public:
    explicit Answer(const AnswerPosition pos);

    void set_invalid();

    void set_answer(const std::string &latin_or_hangul);

    bool is_hovering(const sf::Vector2f mouse_pos) const;

    void toggle_hover_highlight(const sf::Vector2f mouse_pos);

    void set_correct_answer_highlight();

    void set_incorrect_answer_highlight();

    void set_selected_wrong_answer_highlight();
};

}  // namespace ui::circles
