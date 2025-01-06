/**
 * @file app.cpp
 */

#include <array>          // for std::array
#include <cstddef>        // for std::size_t
#include <optional>       // for std::optional
#include <string>         // for std::string
#include <unordered_map>  // for std::unordered_map

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <fmt/core.h>

#include "app.hpp"
#include "core/rng.hpp"
#include "core/settings/colors.hpp"
#include "core/settings/screen.hpp"
#include "core/string.hpp"
#include "modules/vocabulary.hpp"
#include "ui/items.hpp"
#include "version.hpp"
#if defined(_WIN32)
#include "core/windows.hpp"
#endif

namespace app {

namespace {

/**
 * @brief Private helper function to get improved context settings for the SFML window.
 *
 * This overwrites the default context settings to improve the rendering quality by enabling anti-aliasing.
 *
 * @param antialiasing Anti-aliasing level (default: 8).
 *
 * @return Improved context settings.
 *
 * @note According to the SFML documentation, "if any of these settings is not supported by the graphics card, SFML tries to find the closest valid match. For example, if 4x anti-aliasing is too high, it tries 2x and then falls back to 0."
 */
[[nodiscard]] sf::ContextSettings get_improved_context_settings(const unsigned int antialiasing = 8)
{
    sf::ContextSettings settings;
    settings.antiAliasingLevel = antialiasing;
    return settings;
}

}  // namespace

void run()
{
    sf::RenderWindow window(
        sf::VideoMode({core::settings::screen::WIDTH, core::settings::screen::HEIGHT}),
        fmt::format("aegyo ({})", PROJECT_VERSION),
        sf::Style::Titlebar | sf::Style::Close,
        sf::State::Windowed,
        get_improved_context_settings());

    // Enable V-Sync to limit the frame rate to the refresh rate of the monitor
    window.setVerticalSyncEnabled(true);

    // Disable key repeat, as we only want one key press to register
    window.setKeyRepeatEnabled(false);

#if defined(_WIN32)
    // Set window titlebar icon (Windows-only)
    // macOS doesn't have titlebar icons, GNU/Linux is DE-dependent
    if (const auto e = core::windows::setup_titlebar_icon(window); e.has_value()) {
        fmt::print(stderr, "Warning: {}\n", *e);
    }
#endif

    // Request focus on the window
    window.requestFocus();

    // Prepare a vocabulary object to manage game entries
    modules::vocabulary::Vocabulary vocabulary_obj;

    // Define category toggles
    // These allow enabling or disabling certain question types
    const std::array<std::string, 4> toggle_labels = {"Vow", "Con", "DCon", "CompV"};
    const std::array<modules::vocabulary::Category, 4> toggle_categories = {
        modules::vocabulary::Category::BasicVowel,
        modules::vocabulary::Category::BasicConsonant,
        modules::vocabulary::Category::DoubleConsonant,
        modules::vocabulary::Category::CompoundVowel};
    std::unordered_map<modules::vocabulary::Category, bool> toggle_states = {
        {modules::vocabulary::Category::BasicVowel, true},
        {modules::vocabulary::Category::BasicConsonant, true},
        {modules::vocabulary::Category::DoubleConsonant, true},
        {modules::vocabulary::Category::CompoundVowel, true}};

    // Create a circle for displaying the current question
    ui::items::QuestionCircle question_circle;

    core::string::Text memo_text;
    memo_text.setCharacterSize(16);
    memo_text.setFillColor(core::settings::colors::text);
    ui::items::set_integer_position(memo_text, {400.f, 270.f});

    ui::items::Percentage percentage_display;

    // Create four circular buttons for answer choices
    std::array<ui::items::AnswerCircle, 4> answer_circles{
        ui::items::AnswerCircle(ui::items::AnswerCirclePosition::TOP_LEFT),
        ui::items::AnswerCircle(ui::items::AnswerCirclePosition::TOP_RIGHT),
        ui::items::AnswerCircle(ui::items::AnswerCirclePosition::BOTTOM_LEFT),
        ui::items::AnswerCircle(ui::items::AnswerCirclePosition::BOTTOM_RIGHT)};

    // Create four toggle buttons for toggling categories on/off
    std::array<sf::RectangleShape, 4> toggle_buttons;
    std::array<core::string::Text, 4> toggle_texts;
    const float total_toggle_width = 4.f * 60.f;
    const float start_x = static_cast<float>(window.getSize().x) - total_toggle_width - 10.f;
    for (std::size_t i = 0; i < 4; ++i) {
        sf::RectangleShape rect_button(sf::Vector2f(50.f, 35.f));
        rect_button.setOutlineColor(core::settings::colors::text);
        rect_button.setOutlineThickness(1.f);
        rect_button.setPosition({start_x + static_cast<float>(i) * 60.f, 10.f});
        if (toggle_states[toggle_categories[i]]) {
            rect_button.setFillColor(core::settings::colors::enabled_toggle);
        }
        else {
            rect_button.setFillColor(core::settings::colors::disabled_toggle);
        }
        toggle_buttons[i] = rect_button;

        core::string::Text label_text;
        label_text.setCharacterSize(14);
        label_text.setFillColor(core::settings::colors::text);
        label_text.setString(toggle_labels[i]);
        const sf::FloatRect text_bounds = label_text.getLocalBounds();
        label_text.setOrigin({text_bounds.position.x + text_bounds.size.x / 2.f,
                              text_bounds.position.y + text_bounds.size.y / 2.f});
        ui::items::set_integer_position(label_text, rect_button.getPosition() + sf::Vector2f(25.f, 17.5f));
        toggle_texts[i] = label_text;
    }

    // Define an enum for the different states of the game
    enum class game_state_t {
        waiting_for_answer,
        show_result,
        no_entries_enabled
    };
    game_state_t current_game_state = game_state_t::waiting_for_answer;

    // Keep track of the correct entry and how often the user is correct
    modules::vocabulary::Entry correct_entry;
    std::size_t correct_index = 0;
    bool is_hangul = true;

    // Update the score display whenever a question is answered

    // Immediately set up the first question
    // Placing logic inline for clarity
    {
        const auto maybe_entry = vocabulary_obj.get_random_enabled_entry(toggle_states);
        if (!maybe_entry.has_value()) {
            // If no categories are enabled, display an 'X' to indicate no entries
            question_circle.set_invalid();
            for (auto &circle : answer_circles) {
                circle.set_invalid();
            }
            current_game_state = game_state_t::no_entries_enabled;
            memo_text.setString("");
        }
        else {
            correct_entry = maybe_entry.value();
            is_hangul = core::rng::RNG::get_random_bool();
            const auto options = vocabulary_obj.generate_enabled_question_options(correct_entry, toggle_states);
            for (std::size_t i = 0; i < 4; ++i) {
                if (options[i].hangul == correct_entry.hangul) {
                    correct_index = i;
                    break;
                }
            }
            question_circle.set_question(is_hangul ? correct_entry.hangul : correct_entry.latin);
            memo_text.setString("");
            for (std::size_t i = 0; i < 4; ++i) {
                answer_circles[i].set_available(is_hangul ? options[i].latin : options[i].hangul);
            }
            current_game_state = game_state_t::waiting_for_answer;
        }
    }

    // Start the main event loop
    while (window.isOpen()) {
        // Close the window if the user requests it
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

            // Handle category toggles when the user releases the mouse button
            // Ensure it's the left mouse button
            else if (const auto *mouseUp = event->getIf<sf::Event::MouseButtonReleased>()) {
                if (mouseUp->button == sf::Mouse::Button::Left) {
                    const sf::Vector2f checkPos(
                        static_cast<float>(mouseUp->position.x),
                        static_cast<float>(mouseUp->position.y));
                    for (std::size_t i = 0; i < toggle_buttons.size(); ++i) {
                        if (toggle_buttons[i].getGlobalBounds().contains(checkPos)) {
                            const bool old_state = toggle_states[toggle_categories[i]];
                            toggle_states[toggle_categories[i]] = !old_state;
                            if (toggle_states[toggle_categories[i]]) {
                                toggle_buttons[i].setFillColor(core::settings::colors::enabled_toggle);
                            }
                            else {
                                toggle_buttons[i].setFillColor(core::settings::colors::disabled_toggle);
                            }
                            // Reset game state and counters whenever toggles change
                            percentage_display.reset();

                            // Re-initialize a new question inline (same code as above)
                            const auto new_entry = vocabulary_obj.get_random_enabled_entry(toggle_states);
                            if (!new_entry.has_value()) {
                                question_circle.set_invalid();
                                current_game_state = game_state_t::no_entries_enabled;
                                for (auto &circle : answer_circles) {
                                    circle.set_invalid();
                                }
                                memo_text.setString("");
                            }
                            else {
                                correct_entry = new_entry.value();
                                is_hangul = core::rng::RNG::get_random_bool();
                                const auto opts =
                                    vocabulary_obj.generate_enabled_question_options(correct_entry, toggle_states);
                                for (std::size_t j = 0; j < 4; ++j) {
                                    if (opts[j].hangul == correct_entry.hangul) {
                                        correct_index = j;
                                        break;
                                    }
                                }
                                question_circle.set_question(is_hangul ? correct_entry.hangul : correct_entry.latin);
                                memo_text.setString("");
                                for (std::size_t j = 0; j < 4; ++j) {
                                    answer_circles[j].set_available(is_hangul ? opts[j].latin : opts[j].hangul);
                                }
                                current_game_state = game_state_t::waiting_for_answer;
                            }
                            break;
                        }
                    }
                }
            }

            // Adjust toggle button outlines based on mouse position
            else if (const auto *mouseMove = event->getIf<sf::Event::MouseMoved>()) {
                for (std::size_t i = 0; i < toggle_buttons.size(); ++i) {
                    sf::Vector2f checkPos(
                        static_cast<float>(mouseMove->position.x),
                        static_cast<float>(mouseMove->position.y));
                    if (toggle_buttons[i].getGlobalBounds().contains(checkPos)) {
                        toggle_buttons[i].setOutlineThickness(2.f);
                    }
                    else {
                        toggle_buttons[i].setOutlineThickness(1.f);
                    }
                }
            }

            // Main game state handling
            if (current_game_state == game_state_t::waiting_for_answer) {

                // Change button color when the mouse moves over them
                if (const auto *mouseMove = event->getIf<sf::Event::MouseMoved>()) {
                    sf::Vector2f checkPos(
                        static_cast<float>(mouseMove->position.x),
                        static_cast<float>(mouseMove->position.y));
                    for (std::size_t i = 0; i < 4; ++i) {
                        answer_circles[i].toggle_hover(checkPos);
                    }
                }
                // If the user releases the mouse, check if they clicked an answer
                else if (const auto *mouseUp = event->getIf<sf::Event::MouseButtonReleased>()) {
                    if (mouseUp->button == sf::Mouse::Button::Left) {
                        sf::Vector2f checkPos(
                            static_cast<float>(mouseUp->position.x),
                            static_cast<float>(mouseUp->position.y));
                        for (std::size_t i = 0; i < 4; ++i) {
                            if (answer_circles[i].is_hovering(checkPos)) {
                                if (i == correct_index) {
                                    percentage_display.add_correct_answer();
                                    answer_circles[i].set_correct_answer();
                                }
                                else {
                                    percentage_display.add_incorrect_answer();
                                    answer_circles[i].set_selected_wrong_answer();
                                    answer_circles[correct_index].set_correct_answer();
                                }
                                for (std::size_t j = 0; j < 4; ++j) {
                                    if (j != i && j != correct_index) {
                                        answer_circles[j].set_incorrect_answer();
                                    }
                                }

                                memo_text.setString(correct_entry.memo);
                                const sf::FloatRect memo_bounds = memo_text.getLocalBounds();
                                memo_text.setOrigin({memo_bounds.position.x + memo_bounds.size.x / 2.f,
                                                     memo_bounds.position.y + memo_bounds.size.y / 2.f});
                                current_game_state = game_state_t::show_result;
                                break;
                            }
                        }
                    }
                }
                // If the user presses a key, check if they selected a numeric shortcut
                else if (const auto *keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                    std::optional<std::size_t> selected_idx;
                    switch (keyPressed->scancode) {
                    case sf::Keyboard::Scan::Num1:
                        selected_idx = 0;
                        break;
                    case sf::Keyboard::Scan::Num2:
                        selected_idx = 1;
                        break;
                    case sf::Keyboard::Scan::Num3:
                        selected_idx = 2;
                        break;
                    case sf::Keyboard::Scan::Num4:
                        selected_idx = 3;
                        break;
                    default:
                        break;
                    }
                    if (selected_idx) {
                        if (selected_idx == correct_index) {
                            percentage_display.add_correct_answer();
                            answer_circles[*selected_idx].set_correct_answer();
                        }
                        else {
                            percentage_display.add_incorrect_answer();
                            answer_circles[*selected_idx].set_selected_wrong_answer();
                            answer_circles[correct_index].set_correct_answer();
                        }
                        for (std::size_t j = 0; j < 4; ++j) {
                            if (j != selected_idx && j != correct_index) {
                                answer_circles[j].set_incorrect_answer();
                            }
                        }

                        memo_text.setString(correct_entry.memo);
                        const sf::FloatRect memo_bounds = memo_text.getLocalBounds();
                        memo_text.setOrigin({memo_bounds.position.x + memo_bounds.size.x / 2.f,
                                             memo_bounds.position.y + memo_bounds.size.y / 2.f});
                        current_game_state = game_state_t::show_result;
                    }
                }
            }
            else if (current_game_state == game_state_t::show_result) {
                // Any mouse click or key press proceeds to the next question
                if ((event->is<sf::Event::MouseButtonReleased>()) || event->is<sf::Event::KeyPressed>()) {
                    memo_text.setString("");
                    // Re-initialize a new question inline
                    const auto maybe_entry = vocabulary_obj.get_random_enabled_entry(toggle_states);
                    if (!maybe_entry.has_value()) {
                        question_circle.set_invalid();
                        current_game_state = game_state_t::no_entries_enabled;
                        for (auto &circle : answer_circles) {
                            circle.set_invalid();
                        }
                        memo_text.setString("");
                    }
                    else {
                        correct_entry = maybe_entry.value();
                        is_hangul = core::rng::RNG::get_random_bool();
                        const auto opts = vocabulary_obj.generate_enabled_question_options(correct_entry, toggle_states);
                        for (std::size_t i = 0; i < 4; ++i) {
                            if (opts[i].hangul == correct_entry.hangul) {
                                correct_index = i;
                                break;
                            }
                        }
                        question_circle.set_question(is_hangul ? correct_entry.hangul : correct_entry.latin);
                        memo_text.setString("");
                        for (std::size_t i = 0; i < 4; ++i) {
                            answer_circles[i].set_available(is_hangul ? opts[i].latin : opts[i].hangul);
                        }
                        current_game_state = game_state_t::waiting_for_answer;
                    }
                }
            }
            else if (current_game_state == game_state_t::no_entries_enabled) {
                // Do nothing until categories are toggled back on
            }
        }

        // Clear the render window and draw the entire interface
        window.clear(core::settings::colors::background);

        question_circle.draw(window);
        if (current_game_state == game_state_t::show_result) {
            window.draw(memo_text);
        }
        for (std::size_t i = 0; i < 4; ++i) {
            answer_circles[i].draw(window);
        }
        percentage_display.draw(window);
        for (std::size_t i = 0; i < toggle_buttons.size(); ++i) {
            window.draw(toggle_buttons[i]);
            window.draw(toggle_texts[i]);
        }

        // Display the newly drawn frame
        window.display();
    }
}

}  // namespace app
