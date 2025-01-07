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

#include "core/assets/font.hpp"

namespace ui::widgets {

/**
 * @brief Class that represents a memo hint below the question circle.
 *
 * On construction, the class positions itself and sets up its appearance.
 */
class Memo {
  public:
    /**
     * @brief Construct a new Memo object.
     */
    explicit Memo();

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
    core::assets::font::Text text_;
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
     */
    explicit Percentage();

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
    core::assets::font::Text text_;

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
