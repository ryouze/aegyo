/**
 * @file app.cpp
 */

#include <array>          // for std::array
#include <cstddef>        // for std::size_t
#include <memory>         // for std::unique_ptr
#include <optional>       // for std::optional
#include <unordered_map>  // for std::unordered_map
#include <vector>         // for std::vector

#include <SFML/Graphics.hpp>
#include <fmt/core.h>

#include "app.hpp"
#include "core/graphics/font.hpp"
#include "core/graphics/settings/colors.hpp"
#include "core/graphics/window.hpp"
#include "core/hangul.hpp"
#include "core/math/rng.hpp"
#include "ui/circles.hpp"
#include "ui/widgets.hpp"

namespace app {

/**
 * @brief Enum that represents the current state of the game.
 */
enum class GameState {
    /**
     * @brief Waiting for the user to select an answer.
     */
    Waiting,

    /**
     * @brief Showing the result of the user's answer.
     */
    ShowingResult,

    /**
     * @brief No entries are available to ask questions.
     *
     * This typically occurs when all categories are disabled.
     */
    NoEntries
};

void run()
{
    // Create SFML window with sane defaults
    std::unique_ptr<sf::RenderWindow> window = core::graphics::window::create();

    // Load embedded NanumGothic font
    std::unique_ptr<sf::Font> font = core::graphics::font::load();

    // Prepare UI elements
    ui::widgets::Percentage percentage_display(*font);  // Top left corner
    std::array<ui::widgets::CategoryButton, 4>
        category_labels{// Top right corner
                        ui::widgets::CategoryButton(*font, 0, "Vow", core::hangul::Category::BasicVowel),
                        ui::widgets::CategoryButton(*font, 1, "Con", core::hangul::Category::BasicConsonant),
                        ui::widgets::CategoryButton(*font, 2, "DCon", core::hangul::Category::DoubleConsonant),
                        ui::widgets::CategoryButton(*font, 3, "CompV", core::hangul::Category::CompoundVowel)};
    ui::circles::Question question_circle(*font);  // Main question circle
    ui::widgets::Memo memo_text(*font);            // Hint below the main question circle
    std::array<ui::circles::Answer, 4>
        answer_circles{// 4 possible answers for the question
                       ui::circles::Answer(*font, ui::circles::AnswerPosition::TopLeft),
                       ui::circles::Answer(*font, ui::circles::AnswerPosition::TopRight),
                       ui::circles::Answer(*font, ui::circles::AnswerPosition::BottomLeft),
                       ui::circles::Answer(*font, ui::circles::AnswerPosition::BottomRight)};

    // Prepare game objects
    GameState current_state = GameState::Waiting;
    core::hangul::Vocabulary vocab;
    core::hangul::Entry correct_entry;
    std::size_t correct_index = 0;
    bool is_hangul = true;

    // Helper to start or reset a question
    const auto initialize_question = [&](const bool reset_score) {
        if (reset_score) {
            percentage_display.reset();
        }

        // Get the current state of the category toggles for all categories
        std::unordered_map<core::hangul::Category, bool> toggle_states;
        for (const auto &label : category_labels) {
            const auto [category, enabled] = label.get_enabled_state();
            toggle_states[category] = enabled;
        }

        // Pick a random entry from the vocabulary
        const std::optional<core::hangul::Entry> maybe_entry = vocab.get_random_enabled_entry(toggle_states);
        if (!maybe_entry.has_value()) {
            // If no entries are available, set invalid state (unsure if this is even possible, but just in case)
            question_circle.set_invalid();
            current_state = GameState::NoEntries;
            for (auto &c : answer_circles) {
                c.set_invalid();
            }
            return;
        }
        correct_entry = maybe_entry.value();
        is_hangul = core::math::rng::get_random_bool();
        const std::vector<core::hangul::Entry> opts = vocab.generate_enabled_question_options(correct_entry, toggle_states);
        for (std::size_t i = 0; i < opts.size(); ++i) {
            if (opts[i].hangul == correct_entry.hangul) {
                correct_index = i;
                break;
            }
        }
        question_circle.set_question(is_hangul ? correct_entry.hangul : correct_entry.latin);
        for (std::size_t i = 0; i < opts.size(); ++i) {
            answer_circles[i].set_answer(is_hangul ? opts[i].latin : opts[i].hangul);
        }
        current_state = GameState::Waiting;
    };

    initialize_question(false);

    // Request focus on the window
    window->requestFocus();

    // Main loop
    while (window->isOpen()) {
        while (const std::optional event = window->pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window->close();
            }
            else if (const sf::Event::MouseButtonReleased *mouseUp = event->getIf<sf::Event::MouseButtonReleased>()) {
                if (mouseUp->button == sf::Mouse::Button::Left) {
                    const sf::Vector2f pos(
                        static_cast<float>(mouseUp->position.x),
                        static_cast<float>(mouseUp->position.y));
                    // Handle toggles
                    for (auto &button : category_labels) {
                        if (button.is_hovering(pos)) {
                            button.set_enabled(!button.get_enabled());  // Inverse of the original
                            initialize_question(true);                  // Always reset
                            break;
                        }
                    }
                    // Handle answers (Waiting state only)
                    if (current_state == GameState::Waiting) {
                        for (std::size_t i = 0; i < 4; ++i) {
                            if (answer_circles[i].is_hovering(pos)) {
                                if (i == correct_index) {
                                    percentage_display.add_correct_answer();
                                    answer_circles[i].set_answer_highlight(ui::circles::AnswerHighlight::Correct);
                                }
                                else {
                                    percentage_display.add_incorrect_answer();
                                    answer_circles[i].set_answer_highlight(ui::circles::AnswerHighlight::SelectedWrong);
                                    answer_circles[correct_index].set_answer_highlight(ui::circles::AnswerHighlight::Correct);
                                }
                                for (std::size_t j = 0; j < 4; ++j) {
                                    if (j != i && j != correct_index) {
                                        answer_circles[j].set_answer_highlight(ui::circles::AnswerHighlight::Incorrect);
                                    }
                                }
                                memo_text.set(correct_entry.memo);
                                current_state = GameState::ShowingResult;
                                break;
                            }
                        }
                    }
                    // Next question if result shown
                    else if (current_state == GameState::ShowingResult) {
                        initialize_question(false);
                    }
                }
            }
            else if (const sf::Event::MouseMoved *mouseMove = event->getIf<sf::Event::MouseMoved>()) {
                const sf::Vector2f pos(
                    static_cast<float>(mouseMove->position.x),
                    static_cast<float>(mouseMove->position.y));
                // Toggle button hover
                for (auto &button : category_labels) {
                    button.set_hover(pos);
                }
                // Answer circle hover
                if (current_state == GameState::Waiting) {
                    for (auto &circle : answer_circles) {
                        circle.toggle_hover_highlight(pos);
                    }
                }
            }
            else if (const sf::Event::KeyPressed *keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (current_state == GameState::Waiting) {
                    std::optional<std::size_t> selected_index;
                    switch (keyPressed->scancode) {
                    case sf::Keyboard::Scancode::Num1:
                        selected_index = 0;
                        break;
                    case sf::Keyboard::Scancode::Num2:
                        selected_index = 1;
                        break;
                    case sf::Keyboard::Scancode::Num3:
                        selected_index = 2;
                        break;
                    case sf::Keyboard::Scancode::Num4:
                        selected_index = 3;
                        break;
                    default:
                        break;
                    }
                    if (selected_index.has_value()) {
                        if (*selected_index == correct_index) {
                            percentage_display.add_correct_answer();
                            answer_circles[*selected_index].set_answer_highlight(ui::circles::AnswerHighlight::Correct);
                        }
                        else {
                            percentage_display.add_incorrect_answer();
                            answer_circles[*selected_index].set_answer_highlight(ui::circles::AnswerHighlight::SelectedWrong);
                            answer_circles[correct_index].set_answer_highlight(ui::circles::AnswerHighlight::Correct);
                        }
                        for (std::size_t j = 0; j < 4; ++j) {
                            if (j != *selected_index && j != correct_index) {
                                answer_circles[j].set_answer_highlight(ui::circles::AnswerHighlight::Incorrect);
                            }
                        }
                        memo_text.set(correct_entry.memo);
                        current_state = GameState::ShowingResult;
                    }
                }
                else if (current_state == GameState::ShowingResult) {
                    initialize_question(false);
                }
            }
        }

        // Draw everything
        window->clear(core::graphics::settings::colors::background::normal);
        question_circle.draw(*window);
        if (current_state == GameState::ShowingResult) {
            // Only draw memo if showing result
            memo_text.draw(*window);
        }
        for (const auto &circle : answer_circles) {
            circle.draw(*window);
        }
        percentage_display.draw(*window);
        for (const auto &label : category_labels) {
            label.draw(*window);
        }
        window->display();
    }
}

}  // namespace app
