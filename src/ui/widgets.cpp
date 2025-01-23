/**
 * @file widets.cpp
 */

#include <string>  // for std::string

#include <SFML/Graphics.hpp>

#include "components/base.hpp"
#include "core/settings/colors.hpp"
#include "core/settings/screen.hpp"
#include "widgets.hpp"

namespace ui::widgets {

Memo::Memo(const sf::Font &font)
    : text_(font)
{
    // Appearance
    this->text_.setCharacterSize(16);
    this->text_.setFillColor(core::settings::colors::text::normal);

    // Position
    this->text_.setPosition({core::settings::screen::CENTER.x,
                             core::settings::screen::CENTER.y - 30.f});
}

void Memo::hide()
{
    this->text_.setString("");
}

void Memo::set(const std::string &str)
{
    this->text_.setString(str);
    this->text_.resetOrigin();
}

void Memo::draw(sf::RenderWindow &window) const
{
    window.draw(this->text_);
}

Percentage::Percentage(const sf::Font &font)
    : text_(font),
      correct_answers_(0),
      total_answers_(0)
{
    // Appearance
    this->text_.setCharacterSize(18);
    this->text_.setFillColor(core::settings::colors::text::normal);

    // Position
    constexpr float top_left_offset = 10.f;  // Offset from the top-left corner
    this->text_.setPosition({core::settings::screen::TOP_LEFT.x + top_left_offset,
                             core::settings::screen::TOP_LEFT.y + top_left_offset});
    this->update_text();
}

void Percentage::add_correct_answer()
{
    ++this->correct_answers_;
    ++this->total_answers_;
    this->update_text();
}

void Percentage::add_incorrect_answer()
{
    ++this->total_answers_;
    this->update_text();
}

void Percentage::reset()
{
    this->correct_answers_ = 0;
    this->total_answers_ = 0;
    this->update_text();
}

void Percentage::draw(sf::RenderWindow &window) const
{
    window.draw(this->text_);
}

void Percentage::update_text()
{
    float percent = 100.f;  // If no answers yet, default to 100%
    if (this->total_answers_ > 0) {
        percent = (static_cast<float>(this->correct_answers_) /
                   static_cast<float>(this->total_answers_)) *
                  100.f;
    }
    this->text_.setString(fmt::format("게임 점수: {:.1f}%", percent));
}

}  // namespace ui::widgets
