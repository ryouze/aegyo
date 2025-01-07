/**
 * @file items.hpp
 *
 * @brief SFML components and items.
 */

#pragma once

#include <cstddef>  // for std::size_t
#include <string>   // for std::string

#include <SFML/Graphics.hpp>
#include <fmt/core.h>

#include "core/settings/colors.hpp"
#include "core/settings/screen.hpp"
#include "core/string.hpp"

namespace ui::items {

class IDrawable {
  public:
    virtual ~IDrawable() = default;
    virtual void draw(sf::RenderWindow &window) const = 0;
};

class CenteredCircleShape : public sf::CircleShape {
  public:
    explicit CenteredCircleShape(const float radius)
        : sf::CircleShape(radius, 100)  // Use 100 points for a smooth circle
    {
        this->sf::CircleShape::setOrigin(sf::Vector2f(radius, radius));
    }
};

class TextItem : public IDrawable {
  public:
    virtual void draw(sf::RenderWindow &window) const override
    {
        window.draw(this->text_);
    }

  protected:
    core::string::Text text_;
};

class Percentage : public TextItem {
  public:
    explicit Percentage()
        : correct_answers_(0),
          total_answers_(0)
    {
        this->text_.setCharacterSize(18);
        this->text_.setFillColor(core::settings::colors::text);

        const float top_left_offset = 10.f;  // Offset from the top-left corner
        const sf::Vector2f position = {
            core::settings::screen::TOP_LEFT.x + top_left_offset,
            core::settings::screen::TOP_LEFT.y + top_left_offset};
        this->text_.setPosition(position);

        this->update_text_();
    }

    void add_correct_answer()
    {
        ++this->correct_answers_;
        ++this->total_answers_;
        this->update_text_();
    }

    void add_incorrect_answer()
    {
        ++this->total_answers_;
        this->update_text_();
    }

    void reset()
    {
        this->correct_answers_ = 0;
        this->total_answers_ = 0;
        this->update_text_();
    }

  private:
    std::size_t correct_answers_;
    std::size_t total_answers_;

    void update_text_()
    {
        float percent = 100.f;  // If no answers yet, default to 100%
        if (this->total_answers_ > 0) {
            percent = (static_cast<float>(this->correct_answers_) /
                       static_cast<float>(this->total_answers_)) *
                      100.f;
        }
        this->text_.setString(fmt::format("게임 점수: {:.1f}%", percent));
    }
};

class Memo : public TextItem {
  public:
    explicit Memo()
    {
        this->text_.setCharacterSize(16);
        this->text_.setFillColor(core::settings::colors::text);

        const sf::Vector2f position = {
            core::settings::screen::CENTER.x,
            core::settings::screen::CENTER.y - 30.f};
        this->text_.setPosition(position);
    }

    void hide()
    {
        this->text_.setString("");
    }

    void set(const std::string &str)
    {
        this->text_.setString(str);
        this->text_.resetOrigin();
    }
};

class CircleItem : public IDrawable {
  public:
    virtual void draw(sf::RenderWindow &window) const override
    {
        window.draw(this->circle_);
        window.draw(this->text_);
    }

  protected:
    CircleItem(const float radius,
               const sf::Color &fill_color)
        : circle_(radius)
    {
        this->circle_.setFillColor(fill_color);
        this->text_.setFillColor(core::settings::colors::text);
    }

    CenteredCircleShape circle_;
    core::string::Text text_;
};

class QuestionCircle : public CircleItem {
  public:
    QuestionCircle()
        : CircleItem(80.f, core::settings::colors::question_circle)
    {
        const sf::Vector2f position = {
            core::settings::screen::CENTER.x,
            core::settings::screen::CENTER.y - 150.f};
        this->circle_.setPosition(position);

        this->text_.setCharacterSize(48);
        this->text_.setPosition(position);
    }

    void set_invalid()
    {
        this->text_.setCharacterSize(72);
        this->text_.setString("X");
        this->text_.resetOrigin();
    }

    void set_question(const std::string &str)
    {
        this->text_.setCharacterSize(48);
        this->text_.setString(str);
        this->text_.resetOrigin();
    }
};

enum class AnswerCirclePosition {
    TOP_LEFT,
    TOP_RIGHT,
    BOTTOM_LEFT,
    BOTTOM_RIGHT
};

class AnswerCircle : public CircleItem {
  public:
    explicit AnswerCircle(const AnswerCirclePosition pos)
        : CircleItem(60.f, core::settings::colors::default_button)
    {
        sf::Vector2f position;
        switch (pos) {
        case AnswerCirclePosition::TOP_LEFT:
            position = core::settings::screen::CENTER + sf::Vector2f(-150.f, 50.f);
            break;
        case AnswerCirclePosition::TOP_RIGHT:
            position = core::settings::screen::CENTER + sf::Vector2f(150.f, 50.f);
            break;
        case AnswerCirclePosition::BOTTOM_LEFT:
            position = core::settings::screen::CENTER + sf::Vector2f(-150.f, 200.f);
            break;
        case AnswerCirclePosition::BOTTOM_RIGHT:
            position = core::settings::screen::CENTER + sf::Vector2f(150.f, 200.f);
            break;
        }

        this->circle_.setPosition(position);
        this->text_.setCharacterSize(28);
        this->text_.setPosition(position);
    }

    void set_invalid()
    {
        this->circle_.setFillColor(core::settings::colors::disabled_toggle);
        this->text_.setString("");
    }

    void set_available(const std::string &latin_or_hangul)
    {
        this->circle_.setFillColor(core::settings::colors::default_button);
        this->text_.setString(latin_or_hangul);

        // TODO: Find out why we have to do this every time we set a new string instead of doing it once in the constructor
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

}  // namespace ui::items
