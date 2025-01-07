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
    explicit Question()
        : ui::components::base::TextInCircle(80.f)
    {
        // Position
        constexpr sf::Vector2f position = {core::settings::screen::CENTER.x,
                                           core::settings::screen::CENTER.y - 150.f};

        // Circle
        this->circle_.setPosition(position);
        this->circle_.setFillColor(core::settings::colors::question_circle);

        // Text
        this->text_.setPosition(position);
        this->text_.setCharacterSize(48);
    }

    void set_invalid()
    {
        this->text_.setCharacterSize(72);
        this->text_.setString("X");
        this->text_.resetOrigin();
    }

    void set_question(const std::string &latin_or_hangul)
    {
        this->text_.setCharacterSize(48);
        this->text_.setString(latin_or_hangul);
        this->text_.resetOrigin();
    }
};

enum class AnswerPosition {
    TopLeft,
    TopRight,
    BottomLeft,
    BottomRight
};

class Answer : public ui::components::base::TextInCircle {
  public:
    explicit Answer(AnswerPosition pos)
        : ui::components::base::TextInCircle(60.f)
    {
        // Position
        sf::Vector2f position;
        switch (pos) {
        case AnswerPosition::TopLeft:
            position = core::settings::screen::CENTER + sf::Vector2f(-150.f, 50.f);
            break;
        case AnswerPosition::TopRight:
            position = core::settings::screen::CENTER + sf::Vector2f(150.f, 50.f);
            break;
        case AnswerPosition::BottomLeft:
            position = core::settings::screen::CENTER + sf::Vector2f(-150.f, 200.f);
            break;
        case AnswerPosition::BottomRight:
            position = core::settings::screen::CENTER + sf::Vector2f(150.f, 200.f);
            break;
        default:
            throw std::runtime_error("Invalid AnswerPosition");
        }

        // Circle
        this->circle_.setPosition(position);
        this->circle_.setFillColor(core::settings::colors::default_button);

        // Text
        this->text_.setPosition(position);
        this->text_.setCharacterSize(28);
    }

    void set_invalid()
    {
        this->circle_.setFillColor(core::settings::colors::disabled_toggle);
        this->text_.setString("");
    }

    void set_answer(const std::string &latin_or_hangul)
    {
        this->circle_.setFillColor(core::settings::colors::default_button);
        this->text_.setString(latin_or_hangul);
        this->text_.resetOrigin();
    }

    bool is_hovering(const sf::Vector2f mouse_pos) const
    {
        return this->circle_.getGlobalBounds().contains(mouse_pos);
    }

    void toggle_hover(const sf::Vector2f mouse_pos)
    {
        if (this->is_hovering(mouse_pos)) {
            this->circle_.setFillColor(core::settings::colors::hover_button);
        }
        else {
            this->circle_.setFillColor(core::settings::colors::default_button);
        }
    }

    void set_correct_answer()
    {
        this->circle_.setFillColor(core::settings::colors::correct_answer);
    }

    void set_incorrect_answer()
    {
        this->circle_.setFillColor(core::settings::colors::incorrect_answer);
    }

    void set_selected_wrong_answer()
    {
        this->circle_.setFillColor(core::settings::colors::selected_wrong_answer);
    }
};

}  // namespace ui::circles
