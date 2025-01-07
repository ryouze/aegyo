/**
 * @file circles.cpp
 */

#include <SFML/Graphics.hpp>

#include "circles.hpp"
#include "core/settings/colors.hpp"

namespace ui::circles {

Question::Question()
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

void Question::set_invalid()
{
    this->text_.setCharacterSize(72);
    this->text_.setString("X");
    this->text_.resetOrigin();
}

void Question::set_question(const std::string &latin_or_hangul)
{
    this->text_.setCharacterSize(48);
    this->text_.setString(latin_or_hangul);
    this->text_.resetOrigin();
}

Answer::Answer(const AnswerPosition pos)
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

void Answer::set_invalid()
{
    this->circle_.setFillColor(core::settings::colors::disabled_toggle);
    this->text_.setString("");
}

void Answer::set_answer(const std::string &latin_or_hangul)
{
    this->circle_.setFillColor(core::settings::colors::default_button);
    this->text_.setString(latin_or_hangul);
    this->text_.resetOrigin();
}

bool Answer::is_hovering(const sf::Vector2f mouse_pos) const
{
    return this->circle_.getGlobalBounds().contains(mouse_pos);
}

void Answer::toggle_hover_highlight(const sf::Vector2f mouse_pos)
{
    if (this->is_hovering(mouse_pos)) {
        this->circle_.setFillColor(core::settings::colors::hover_button);
    }
    else {
        this->circle_.setFillColor(core::settings::colors::default_button);
    }
}

void Answer::set_correct_answer_highlight()
{
    this->circle_.setFillColor(core::settings::colors::correct_answer);
}

void Answer::set_incorrect_answer_highlight()
{
    this->circle_.setFillColor(core::settings::colors::incorrect_answer);
}

void Answer::set_selected_wrong_answer_highlight()
{
    this->circle_.setFillColor(core::settings::colors::selected_wrong_answer);
}

}  // namespace ui::circles
