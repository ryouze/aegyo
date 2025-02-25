/**
 * @file widgets.hpp
 *
 * @brief Standalone widgets for the user interface.
 */

#pragma once

#include <cstddef>  // for std::size_t
#include <string>   // for std::string

#include <SFML/Graphics.hpp>
#include <fmt/core.h>

#include "core/graphics/settings/screen.hpp"
#include "core/hangul.hpp"
#include "core/shapes.hpp"

namespace ui::widgets {

class CategoryLabel {
  public:
    explicit CategoryLabel(const sf::Font &font,
                           const std::size_t order,
                           const std::string &label,
                           const core::hangul::Category category,
                           const bool enabled = true)
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
        this->button_.setPosition({core::graphics::settings::screen::TOP_RIGHT.x - padding - (button_size.x + padding) * (4 - order),
                                   core::graphics::settings::screen::TOP_RIGHT.y + padding});

        // Text appearance
        this->text_.setCharacterSize(14);
        this->text_.setFillColor(core::graphics::settings::colors::text::normal);
        this->text_.setString(label);

        // Text position
        this->text_.resetOrigin();
        this->text_.setPosition(this->button_.getPosition() + button_size / 2.f);
    }

    std::tuple<core::hangul::Category, bool> get_toggle_state()
    {
        return std::make_tuple(this->category_, this->enabled_);
    }

    /**
     * @brief Check if the mouse is hovering over the category button.
     *
     * @param mouse_pos Mouse position.
     *
     * @return True if the mouse is hovering over the category button, false otherwise.
     */
    [[nodiscard]] bool is_hovering(const sf::Vector2f mouse_pos) const
    {
        return this->button_.getGlobalBounds().contains(mouse_pos);
    }

    void set(const bool enabled)
    {
        this->enabled_ = enabled;
        this->button_.setFillColor(enabled ? core::graphics::settings::colors::category::enabled : core::graphics::settings::colors::category::disabled);
    }

    void set_thickness(const bool hover)
    {
        this->button_.setOutlineThickness(hover ? 2.f : 1.f);
    }

    void draw(sf::RenderWindow &window) const
    {
        window.draw(this->button_);
        window.draw(this->text_);
    }

  private:
    sf::RectangleShape button_;

    /**
     * @brief Text object.
     */
    core::shapes::Text text_;

    /**
     * @brief Whether this button is enabled (e.g., "true").
     */
    bool enabled_;

    /**
     * @brief Vocabulary category (e.g., "BasicVowel").
     *
     */
    core::hangul::Category category_;
};

/**
 * @brief Class that represents a memo hint below the question circle.
 *
 * On construction, the class positions itself and sets up its appearance.
 */
class Memo {
  public:
    /**
     * @brief Construct a new Memo object.
     *
     * @param font Font to use for the memo text.
     */
    explicit Memo(const sf::Font &font);

    /**
     * @brief Hide the memo text.
     */
    void hide();

    /**
     * @brief Set the memo text to a string.
     *
     * @param str Text to set (e.g., "This is a memo").
     */
    void set(const std::string &str);

    /**
     * @brief Draw the text to the window.
     *
     * @param window Window to draw to.
     */
    void draw(sf::RenderWindow &window) const;

  private:
    /**
     * @brief Text object.
     */
    core::shapes::Text text_;
};

/**
 * @brief Class that represents the percentage of correct answers.
 *
 * On construction, the class positions itself and sets up its appearance.
 */
class Percentage {
  public:
    /**
     * @brief Construct a new Percentage object.
     *
     * @param font Font to use for the percentage text.
     */
    explicit Percentage(const sf::Font &font);

    /**
     * @brief Add a correct answer to the total and update the text.
     */
    void add_correct_answer();

    /**
     * @brief Add an incorrect answer to the total and update the text.
     */
    void add_incorrect_answer();

    /**
     * @brief Reset the correct/total answers to zero and update the text.
     */
    void reset();

    /**
     * @brief Draw the text to the window.
     *
     * @param window Window to draw to.
     */
    void draw(sf::RenderWindow &window) const;

  private:
    /**
     * @brief Update the text to show the current percentage of correct answers.
     *
     * If no answers have been given yet, the percentage defaults to 100%.
     */
    void update_text();

    /**
     * @brief Text object.
     */
    core::shapes::Text text_;

    /**
     * @brief Number of correct answers (e.g., "10").
     */
    std::size_t correct_answers_;

    /**
     * @brief Total number of answers (e.g., "15").
     */
    std::size_t total_answers_;
};

}  // namespace ui::widgets
