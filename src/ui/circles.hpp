/**
 * @file circles.hpp
 *
 * @brief Question and answer circles.
 */

#pragma once

#include <string>  // for std::string

#include <SFML/Graphics.hpp>

#include "core/shapes.hpp"

namespace ui::circles {

/**
 * @brief Class that represents the main question circle in the top center of the screen.
 *
 * On construction, the class positions itself and sets up its appearance.
 */
class Question : public core::shapes::BaseCircleWithText {
  public:
    /**
     * @brief Construct a new Question object.
     *
     * @param font Font to use for the question circle.
     */
    explicit Question(const sf::Font &font);

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
 * @brief Enum that that maps to the highlight color of the answer circle.
 */
enum class AnswerHighlight {
    /**
     * @brief Correct answer (green).
     *
     * Whether the user clicked on the correct answer or not, the correct answer will always be highlighted in green.
     */
    Correct,

    /**
     * @brief Selected wrong answer (orange).
     *
     * If the user clicked on the wrong answer, the wrong answer will be highlighted in orange for visual feedback, but it is basically the same as Incorrect.
     */
    SelectedWrong,

    /**
     * @brief Incorrect answer (red).
     *
     * Any remaining incorrect answers will be highlighted in red.
     */
    Incorrect,
};

/**
 * @brief Class that represents one of the four answer circles.
 *
 * On construction, the class positions itself and sets up its appearance.
 */
class Answer : public core::shapes::BaseCircleWithText {
  public:
    /**
     * @brief Construct a new Answer object.
     *
     * Each answer should be positioned in one of the four corners of the screen, with no repetition. The order doesn't matter.
     *
     * @param font Font to use for the answer circle.
     * @param pos Position of the answer circle (e.g., AnswerPosition::TopLeft).
     *
     * @throws std::runtime_error if the position is invalid.
     */
    explicit Answer(const sf::Font &font,
                    const AnswerPosition pos);

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
     * @brief Set the highlight of the answer circle to indicate the correct answer.
     *
     * The following colors are used:
     * - Correct: Green.
     * - Incorrect: Red.
     * - Wrong: Orange.
     *
     * @param highlight Highlight color to set (e.g., "AnswerHighlight::Correct").
     */
    void set_answer_highlight(const AnswerHighlight highlight);
};

}  // namespace ui::circles
