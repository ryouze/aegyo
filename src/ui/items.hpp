/**
 * @file items.hpp
 *
 * @brief SFML components and items.
 */

#pragma once

#include <cstddef>  // for std::size_t

#include <SFML/Graphics.hpp>
#include <fmt/core.h>

#include "core/settings/colors.hpp"
#include "core/settings/screen.hpp"
#include "core/string.hpp"

namespace ui::items {

class Percentage {
  public:
    explicit Percentage()
        : correct_answers_(0),
          total_answers_(0)
    {
        this->text_.setCharacterSize(18);
        this->text_.setFillColor(core::settings::colors::text);

        const float top_left_offset = 10.f;  // Offset from the top-left corner
        const sf::Vector2f position = sf::Vector2f(core::settings::screen::TOP_LEFT.x + top_left_offset,
                                                   core::settings::screen::TOP_LEFT.y + top_left_offset);
        this->text_.setPosition(position);
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
    core::string::Text text_;
    std::size_t correct_answers_ = 0;
    std::size_t total_answers_ = 0;

    void update_score_display()
    {
        float percentage = 100.f;  // If no answers yet, default to 100%
        if (this->total_answers_ > 0) {
            percentage = (static_cast<float>(this->correct_answers_) / static_cast<float>(this->total_answers_)) * 100.f;
        }

        this->text_.setString(fmt::format("게임 점수: {:.1f}%", percentage));
    }
};

class Memo {
  public:
    explicit Memo()
    {
        this->text_.setCharacterSize(16);
        this->text_.setFillColor(core::settings::colors::text);
        const sf::Vector2f position = sf::Vector2f(core::settings::screen::CENTER.x + 0.f,
                                                   core::settings::screen::CENTER.y + 30.f);
        this->text_.setPosition(position);
    }

    void hide()
    {
        this->text_.setString("");
    }

    void set(const std::string &memo)
    {
        this->text_.setString(memo);
        this->text_.resetOrigin();
    }

    void draw(sf::RenderWindow &window) const
    {
        window.draw(this->text_);
    }

  private:
    core::string::Text text_;
};

class Circle : public sf::CircleShape {
  public:
    explicit Circle(const float radius)
        : sf::CircleShape(radius, 100)  // Use 100 points for a smooth circle
    {
        // Set origin to the center of the circle instead of the default top-left corner
        sf::CircleShape::setOrigin({radius, radius});
    }
};

class QuestionCircle {
  public:
    explicit QuestionCircle()
        : circle_(80.f)
    {
        // Setup circle
        this->circle_.setFillColor(core::settings::colors::question_circle);
        const sf::Vector2f position = core::settings::screen::CENTER + sf::Vector2f(0.f, -150.f);
        this->circle_.setPosition(position);

        // Setup text
        this->text_.setCharacterSize(48);
        this->text_.setFillColor(core::settings::colors::text);
        this->text_.setPosition(position);
    }

    void set_invalid()
    {
        this->text_.setString("X");
        this->text_.setCharacterSize(72);
        this->text_.resetOrigin();
        ;
    }

    void set_question(const std::string &latin_or_hangul)
    {
        this->text_.setCharacterSize(48);
        this->text_.setString(latin_or_hangul);
        this->text_.resetOrigin();
        ;
    }

    void draw(sf::RenderWindow &window) const
    {
        window.draw(this->circle_);
        window.draw(this->text_);
    }

  private:
    Circle circle_;
    core::string::Text text_;
};

enum class AnswerCirclePosition {
    TOP_LEFT,
    TOP_RIGHT,
    BOTTOM_LEFT,
    BOTTOM_RIGHT
};

class AnswerCircle {
  public:
    explicit AnswerCircle(const AnswerCirclePosition coordinate)
        : circle_(60.f)
    {
        // Setup circle
        this->circle_.setFillColor(core::settings::colors::default_button);
        sf::Vector2f position;
        switch (coordinate) {
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

        // Setup text
        this->text_.setCharacterSize(28);
        this->text_.setFillColor(core::settings::colors::text);
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
        ;
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

    void draw(sf::RenderWindow &window) const
    {
        window.draw(this->circle_);
        window.draw(this->text_);
    }

  private:
    Circle circle_;
    core::string::Text text_;
};

}  // namespace ui::items
