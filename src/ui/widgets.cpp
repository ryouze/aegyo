/**
 * @file widets.cpp
 */

#include <cstddef>  // for std::size_t
#include <string>   // for std::string
#include <tuple>    // for std::tuple, std::make_tuple

#include <SFML/Graphics.hpp>

#include "core/graphics/settings/colors.hpp"
#include "core/graphics/settings/screen.hpp"
#include "core/shapes.hpp"
#include "widgets.hpp"

namespace ui::widgets {

Memo::Memo(const sf::Font &font)
    : text_(font)
{
    // Appearance
    this->text_.setCharacterSize(16);
    this->text_.setFillColor(core::graphics::settings::colors::text::normal);

    // Position
    this->text_.setPosition({core::graphics::settings::screen::CENTER.x,
                             core::graphics::settings::screen::CENTER.y - 30.f});
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
    this->text_.setFillColor(core::graphics::settings::colors::text::normal);

    // Position
    constexpr float top_left_offset = 10.f;  // Offset from the top-left corner
    this->text_.setPosition({core::graphics::settings::screen::TOP_LEFT.x + top_left_offset,
                             core::graphics::settings::screen::TOP_LEFT.y + top_left_offset});
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

CategoryButton::CategoryButton(const sf::Font &font,
                               const std::size_t order,
                               const std::string &label,
                               const core::hangul::Category category,
                               const bool enabled)
    : button_(sf::Vector2f(50.f, 35.f)),
      text_(font, label),
      enabled_(enabled),
      category_(category)
{
    // Button appearance
    this->button_.setOutlineColor(core::graphics::settings::colors::text::normal);
    this->button_.setOutlineThickness(1.f);
    this->button_.setFillColor(enabled ? core::graphics::settings::colors::category::enabled : core::graphics::settings::colors::category::disabled);

    // Button position
    const sf::Vector2f button_size = this->button_.getSize();
    constexpr float padding = 10.f;
    this->button_.setPosition({core::graphics::settings::screen::TOP_RIGHT.x - padding - (button_size.x + padding) * static_cast<float>(4 - order),
                               core::graphics::settings::screen::TOP_RIGHT.y + padding});

    // Text appearance
    this->text_.setCharacterSize(14);
    this->text_.setFillColor(core::graphics::settings::colors::text::normal);
    this->text_.setString(label);

    // Text position
    this->text_.resetOrigin();
    this->text_.setPosition(this->button_.getPosition() + button_size / 2.f);
}

std::tuple<core::hangul::Category, bool> CategoryButton::get_enabled_state() const
{
    return std::make_tuple(this->category_, this->enabled_);
}

bool CategoryButton::get_enabled() const
{
    return this->enabled_;
}

void CategoryButton::set_enabled(const bool enabled)
{
    this->enabled_ = enabled;
    this->button_.setFillColor(
        enabled
            // If enabled, then enabled color, otherwise disabled color
            ? core::graphics::settings::colors::category::enabled
            : core::graphics::settings::colors::category::disabled);
}

bool CategoryButton::is_hovering(const sf::Vector2f mouse_pos) const
{
    return this->button_.getGlobalBounds().contains(mouse_pos);
}

void CategoryButton::set_hover(const sf::Vector2f mouse_pos)
{
    this->button_.setOutlineThickness(
        this->is_hovering(mouse_pos)
            // If hovering, make thicker
            ? 2.f
            : 1.f);
}

void CategoryButton::draw(sf::RenderWindow &window) const
{
    window.draw(this->button_);
    window.draw(this->text_);
}

}  // namespace ui::widgets
