/**
 * @file widgets.hpp
 *
 * @brief Standalone widgets for the user interface.
 */

#pragma once

#include <cstddef>  // for std::size_t
#include <string>   // for std::string
#include <tuple>    // for std::tuple

#include <SFML/Graphics.hpp>
#include <fmt/core.h>

#include "core/graphics/settings/screen.hpp"
#include "core/hangul.hpp"
#include "core/shapes.hpp"

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

/**
 * @brief Class that represents a category label button, which can be toggled on and off.
 *
 * On construction, the class positions itself and sets up its appearance.
 */
class CategoryButton {
  public:
    /**
     * @brief Construct a new Category Label object.
     *
     * Each category label should be positioned in the top right corner of the screen. The order doesn't matter.
     *
     * @param font Font to use for the category label.
     * @param order Current order of the category label, must "0", "1", "2", or "3" (e.g., "1").
     * @param label Text within the button (e.g., "CompV").
     * @param category Hangul category (e.g., "Category::CompoundVowel").
     */
    explicit CategoryButton(const sf::Font &font,
                            const std::size_t order,
                            const std::string &label,
                            const core::hangul::Category category,
                            const bool enabled = true);

    /**
     * @brief Get the category and enabled state of the button.
     *
     * @return Tuple containing the category and enabled state (e.g., "{Category::CompoundVowel, true}").
     */
    [[nodiscard]] std::tuple<core::hangul::Category, bool> get_enabled_state() const;

    /**
     * @brief Get the state of the category button (enabled or disabled).
     *
     * @return True if the button is enabled, false otherwise.
     */
    [[nodiscard]] bool get_enabled() const;

    /**
     * @brief Set the state of the category button to enabled or disabled.
     *
     * This sets the internal boolean flag and changes the appearance of the button.
     *
     * @param enabled Whether the button should be enabled (e.g., "true").
     */
    void set_enabled(const bool enabled);

    /**
     * @brief Check if the mouse is hovering over the category button.
     *
     * @param mouse_pos Mouse position.
     *
     * @return True if the mouse is hovering over the category button, false otherwise.
     */
    [[nodiscard]] bool is_hovering(const sf::Vector2f mouse_pos) const;

    /**
     * @brief Set the hover state of the button.
     *
     * If the mouse is hovering over the button, the outline thickness is increased. Otherwise, it is set to the default value.
     *
     * @param mouse_pos Mouse position.
     */
    void set_hover(const sf::Vector2f mouse_pos);

    /**
     * @brief Draw the button to the window.
     *
     * @param window Window to draw to.
     */
    void draw(sf::RenderWindow &window) const;

  private:
    /**
     * @brief Button rectangle.
     */
    sf::RectangleShape button_;

    /**
     * @brief Text label within the button.
     */
    core::shapes::Text text_;

    /**
     * @brief Whether this button is enabled (e.g., "true").
     */
    bool enabled_;

    /**
     * @brief Vocabulary category (e.g., "BasicVowel").
     */
    core::hangul::Category category_;
};

}  // namespace ui::widgets
