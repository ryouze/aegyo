/**
 * @file circles.hpp
 *
 * @brief Question and answer circles.
 */

#pragma once

#include <string>  // for std::string

#include <SFML/Graphics.hpp>

#include "components/base.hpp"

namespace ui::circles {

/**
 * @brief Class that represents the main question circle in the top center of the screen.
 *
 * On construction, the class positions itself and sets up its appearance.
 */
class Question : public ui::components::base::TextInCircle {
  public:
    /**
     * @brief Construct a new Question object.
     */
    explicit Question();

    /**
     * @brief Set the question circle to an invalid state.
     *
     * This enlarges the text and sets it to "X".
     */
    void set_invalid();

    /**
     * @brief Set the question text to the specified Latin or Hangul text.
     *
     * @param latin_or_hangul Latin or Hangul text to set (e.g., "a").
     */
    void set_question(const std::string &latin_or_hangul);
};

/**
 * @brief Enum that represents the position of one of the four answer circles.
 *
 * The position is calculated relatively to the center of the screen.
 */
enum class AnswerPosition {
    /**
     * @brief Top-left answer circle.
     */
    TopLeft,

    /**
     * @brief Top-right answer circle.
     */
    TopRight,

    /**
     * @brief Bottom-left answer circle.
     */
    BottomLeft,

    /**
     * @brief Bottom-right answer circle.
     */
    BottomRight
};

/**
 * @brief Class that represents one of the four answer circles.
 *
 * On construction, the class positions itself and sets up its appearance.
 */
class Answer : public ui::components::base::TextInCircle {
  public:
    /**
     * @brief Construct a new Answer object.
     *
     * @param pos Position of the answer circle (e.g., AnswerPosition::TopLeft).
     *
     * Each answer should be positioned in one of the four corners of the screen, with no repetition. The order doesn't matter.
     */
    explicit Answer(const AnswerPosition pos);

    /**
     * @brief Set the answer circle to an invalid state.
     *
     * This disables the text and sets the circle to a different color.
     */
    void set_invalid();

    /**
     * @brief Set the answer text to the specified Latin or Hangul text.
     *
     * @param latin_or_hangul Latin or Hangul text to set (e.g., "a").
     */
    void set_answer(const std::string &latin_or_hangul);

    /**
     * @brief Check if the mouse is hovering over the answer circle.
     *
     * @param mouse_pos Mouse position.
     *
     * @return True if the mouse is hovering over the answer circle, false otherwise.
     */
    [[nodiscard]] bool is_hovering(const sf::Vector2f mouse_pos) const;

    /**
     * @brief Toggle the hover highlight of the answer circle based on the mouse position.
     *
     * If the provided mouse position is inside the circle, the circle will be highlighted. Otherwise, the circle will be reset to its default appearance.
     *
     * @param mouse_pos Mouse position.
     */
    void toggle_hover_highlight(const sf::Vector2f mouse_pos);

    /**
     * @brief Set the highlight of the answer circle to indicate the correct answer (green).
     */
    void set_correct_answer_highlight();

    /**
     * @brief Set the highlight of the answer circle to indicate an incorrect answer (red).
     */
    void set_incorrect_answer_highlight();

    /**
     * @brief Set the highlight of the answer circle to indicate a selected wrong answer (orange).
     */
    void set_selected_wrong_answer_highlight();
};

}  // namespace ui::circles
