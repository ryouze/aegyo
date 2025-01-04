/**
 * @file app.cpp
 */

#include <array>          // for std::array
#include <cstddef>        // for std::size_t
#include <limits>         // for std::numeric_limits
#include <string>         // for std::string
#include <unordered_map>  // for std::unordered_map

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <fmt/core.h>

#include "app.hpp"
#include "core/assets/font.hpp"
#include "core/rng.hpp"
#include "core/settings/screen.hpp"
#include "core/string.hpp"
#include "core/text.hpp"
#include "modules/vocabulary.hpp"
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

    // Define basic colors used throughout
    const sf::Color color_background(30, 30, 30);
    const sf::Color color_text(240, 240, 240);
    const sf::Color color_enabled_toggle(100, 200, 100);
    const sf::Color color_disabled_toggle(200, 100, 100);
    const sf::Color color_default_button(80, 80, 80);
    const sf::Color color_hover_button(100, 100, 100);
    const sf::Color color_correct_answer(30, 130, 30);
    const sf::Color color_incorrect_answer(130, 30, 30);
    const sf::Color color_selected_wrong_answer(200, 120, 0);
    const sf::Color color_question_circle(50, 50, 50);

    // Create a circle for displaying the current question
    sf::CircleShape question_circle(80.f, 100);
    question_circle.setFillColor(color_question_circle);
    question_circle.setOrigin({80.f, 80.f});
    question_circle.setPosition({400.f, 150.f});

    // Create text elements for the question, memo, and percentage score
    sf::Text question_text(core::assets::font::get_embedded_font());
    question_text.setFont(core::assets::font::get_embedded_font());
    question_text.setCharacterSize(48);
    question_text.setFillColor(color_text);
    core::text::set_integer_position(question_text, question_circle.getPosition());

    sf::Text memo_text(core::assets::font::get_embedded_font());
    memo_text.setFont(core::assets::font::get_embedded_font());
    memo_text.setCharacterSize(16);
    memo_text.setFillColor(color_text);
    core::text::set_integer_position(memo_text, {400.f, 270.f});

    sf::Text percentage_text(core::assets::font::get_embedded_font());
    percentage_text.setFont(core::assets::font::get_embedded_font());
    percentage_text.setCharacterSize(18);
    percentage_text.setFillColor(color_text);
    core::text::set_integer_position(percentage_text, {10.f, 10.f});

    // Create four circular buttons for answer choices
    std::array<sf::CircleShape, 4> button_shapes;
    std::array<sf::Text, 4> answer_texts = {
        sf::Text(core::assets::font::get_embedded_font()),
        sf::Text(core::assets::font::get_embedded_font()),
        sf::Text(core::assets::font::get_embedded_font()),
        sf::Text(core::assets::font::get_embedded_font())};
    const float button_radius = 60.f;
    for (std::size_t i = 0; i < 4; ++i) {
        button_shapes[i].setRadius(button_radius);
        button_shapes[i].setPointCount(100);
        button_shapes[i].setFillColor(color_default_button);
        button_shapes[i].setOrigin({button_radius, button_radius});

        answer_texts[i].setFont(core::assets::font::get_embedded_font());
        answer_texts[i].setCharacterSize(28);
        answer_texts[i].setFillColor(color_text);
    }
    button_shapes[0].setPosition({250.f, 350.f});
    button_shapes[1].setPosition({550.f, 350.f});
    button_shapes[2].setPosition({250.f, 500.f});
    button_shapes[3].setPosition({550.f, 500.f});
    for (std::size_t i = 0; i < 4; ++i) {
        core::text::set_integer_position(answer_texts[i], button_shapes[i].getPosition());
    }

    // Create four toggle buttons for toggling categories on/off
    std::array<sf::RectangleShape, 4> toggle_buttons;
    std::array<sf::Text, 4> toggle_texts = {
        sf::Text(core::assets::font::get_embedded_font()),
        sf::Text(core::assets::font::get_embedded_font()),
        sf::Text(core::assets::font::get_embedded_font()),
        sf::Text(core::assets::font::get_embedded_font())};
    const float total_toggle_width = 4.f * 60.f;
    const float start_x = static_cast<float>(window.getSize().x) - total_toggle_width - 10.f;
    for (std::size_t i = 0; i < 4; ++i) {
        sf::RectangleShape rect_button(sf::Vector2f(50.f, 35.f));
        rect_button.setOutlineColor(color_text);
        rect_button.setOutlineThickness(1.f);
        rect_button.setPosition({start_x + static_cast<float>(i) * 60.f, 10.f});
        if (toggle_states[toggle_categories[i]]) {
            rect_button.setFillColor(color_enabled_toggle);
        }
        else {
            rect_button.setFillColor(color_disabled_toggle);
        }
        toggle_buttons[i] = rect_button;

        sf::Text label_text(core::assets::font::get_embedded_font());
        label_text.setFont(core::assets::font::get_embedded_font());
        label_text.setCharacterSize(14);
        label_text.setFillColor(color_text);
        label_text.setString(toggle_labels[i]);
        const sf::FloatRect text_bounds = label_text.getLocalBounds();
        label_text.setOrigin({text_bounds.position.x + text_bounds.size.x / 2.f,
                              text_bounds.position.y + text_bounds.size.y / 2.f});
        core::text::set_integer_position(label_text, rect_button.getPosition() + sf::Vector2f(25.f, 17.5f));
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
    std::size_t total_questions = 0;
    std::size_t correct_answers = 0;

    // Update the score display whenever a question is answered
    {
        const float percentage = total_questions > 0
                                     ? (static_cast<float>(correct_answers) / static_cast<float>(total_questions)) * 100.f
                                     : 0.f;
        const auto percentage_str = fmt::format("게임 점수: {:.1f}%", percentage);
        percentage_text.setString(core::string::to_sfml_string(percentage_str));
    }

    // Immediately set up the first question
    // Placing logic inline for clarity
    {
        const auto maybe_entry = vocabulary_obj.get_random_enabled_entry(toggle_states);
        if (!maybe_entry.has_value()) {
            // If no categories are enabled, display an 'X' to indicate no entries
            question_text.setString("X");
            question_text.setCharacterSize(72);
            const sf::FloatRect text_bounds = question_text.getLocalBounds();
            question_text.setOrigin({text_bounds.position.x + text_bounds.size.x / 2.f,
                                     text_bounds.position.y + text_bounds.size.y / 2.f});
            current_game_state = game_state_t::no_entries_enabled;
            for (auto &shape : button_shapes) {
                shape.setFillColor(color_disabled_toggle);
            }
            for (auto &text : answer_texts) {
                text.setString("");
            }
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
            question_text.setCharacterSize(48);
            question_text.setString(core::string::to_sfml_string(
                is_hangul ? correct_entry.hangul : correct_entry.latin));
            const sf::FloatRect text_bounds = question_text.getLocalBounds();
            question_text.setOrigin({text_bounds.position.x + text_bounds.size.x / 2.f,
                                     text_bounds.position.y + text_bounds.size.y / 2.f});
            memo_text.setString("");
            for (std::size_t i = 0; i < 4; ++i) {
                button_shapes[i].setFillColor(color_default_button);
                answer_texts[i].setString(core::string::to_sfml_string(
                    is_hangul ? options[i].latin : options[i].hangul));
                const sf::FloatRect ans_bounds = answer_texts[i].getLocalBounds();
                answer_texts[i].setOrigin({ans_bounds.position.x + ans_bounds.size.x / 2.f,
                                           ans_bounds.position.y + ans_bounds.size.y / 2.f});
                core::text::set_integer_position(answer_texts[i], button_shapes[i].getPosition());
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
                                toggle_buttons[i].setFillColor(color_enabled_toggle);
                            }
                            else {
                                toggle_buttons[i].setFillColor(color_disabled_toggle);
                            }
                            // Reset game state and counters whenever toggles change
                            total_questions = 0;
                            correct_answers = 0;
                            {
                                const float percentage = 0.f;
                                const auto percentage_str = fmt::format("게임 점수: {:.1f}%", percentage);
                                percentage_text.setString(core::string::to_sfml_string(percentage_str));
                            }
                            // Re-initialize a new question inline (same code as above)
                            const auto new_entry = vocabulary_obj.get_random_enabled_entry(toggle_states);
                            if (!new_entry.has_value()) {
                                question_text.setString("X");
                                question_text.setCharacterSize(72);
                                const sf::FloatRect tb = question_text.getLocalBounds();
                                question_text.setOrigin({tb.position.x + tb.size.x / 2.f,
                                                         tb.position.y + tb.size.y / 2.f});
                                current_game_state = game_state_t::no_entries_enabled;
                                for (auto &shape : button_shapes) {
                                    shape.setFillColor(color_disabled_toggle);
                                }
                                for (auto &txt : answer_texts) {
                                    txt.setString("");
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
                                question_text.setCharacterSize(48);
                                question_text.setString(core::string::to_sfml_string(
                                    is_hangul ? correct_entry.hangul : correct_entry.latin));
                                const sf::FloatRect tb = question_text.getLocalBounds();
                                question_text.setOrigin({tb.position.x + tb.size.x / 2.f,
                                                         tb.position.y + tb.size.y / 2.f});
                                memo_text.setString("");
                                for (std::size_t j = 0; j < 4; ++j) {
                                    button_shapes[j].setFillColor(color_default_button);
                                    answer_texts[j].setString(core::string::to_sfml_string(
                                        is_hangul ? opts[j].latin : opts[j].hangul));
                                    const sf::FloatRect ans_bounds = answer_texts[j].getLocalBounds();
                                    answer_texts[j].setOrigin({ans_bounds.position.x + ans_bounds.size.x / 2.f,
                                                               ans_bounds.position.y + ans_bounds.size.y / 2.f});
                                    core::text::set_integer_position(answer_texts[j], button_shapes[j].getPosition());
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
                        if (button_shapes[i].getGlobalBounds().contains(checkPos)) {
                            button_shapes[i].setFillColor(color_hover_button);
                        }
                        else {
                            button_shapes[i].setFillColor(color_default_button);
                        }
                    }
                }
                // If the user releases the mouse, check if they clicked an answer
                else if (const auto *mouseUp = event->getIf<sf::Event::MouseButtonReleased>()) {
                    if (mouseUp->button == sf::Mouse::Button::Left) {
                        sf::Vector2f checkPos(
                            static_cast<float>(mouseUp->position.x),
                            static_cast<float>(mouseUp->position.y));
                        for (std::size_t i = 0; i < 4; ++i) {
                            if (button_shapes[i].getGlobalBounds().contains(checkPos)) {
                                ++total_questions;
                                if (i == correct_index) {
                                    ++correct_answers;
                                    button_shapes[i].setFillColor(color_correct_answer);
                                }
                                else {
                                    button_shapes[i].setFillColor(color_selected_wrong_answer);
                                    button_shapes[correct_index].setFillColor(color_correct_answer);
                                }
                                for (std::size_t j = 0; j < 4; ++j) {
                                    if (j != i && j != correct_index) {
                                        button_shapes[j].setFillColor(color_incorrect_answer);
                                    }
                                }
                                {
                                    const float percentage =
                                        static_cast<float>(correct_answers) / static_cast<float>(total_questions) * 100.f;
                                    const auto percentage_str = fmt::format("게임 점수: {:.1f}%", percentage);
                                    percentage_text.setString(core::string::to_sfml_string(percentage_str));
                                }
                                memo_text.setString(core::string::to_sfml_string(correct_entry.memo));
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
                    std::size_t selected_idx = std::numeric_limits<std::size_t>::max();
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
                    if (selected_idx < 4) {
                        ++total_questions;
                        if (selected_idx == correct_index) {
                            ++correct_answers;
                            button_shapes[selected_idx].setFillColor(color_correct_answer);
                        }
                        else {
                            button_shapes[selected_idx].setFillColor(color_selected_wrong_answer);
                            button_shapes[correct_index].setFillColor(color_correct_answer);
                        }
                        for (std::size_t j = 0; j < 4; ++j) {
                            if (j != selected_idx && j != correct_index) {
                                button_shapes[j].setFillColor(color_incorrect_answer);
                            }
                        }
                        {
                            const float percentage =
                                static_cast<float>(correct_answers) / static_cast<float>(total_questions) * 100.f;
                            const auto percentage_str = fmt::format("게임 점수: {:.1f}%", percentage);
                            percentage_text.setString(core::string::to_sfml_string(percentage_str));
                        }
                        memo_text.setString(core::string::to_sfml_string(correct_entry.memo));
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
                        question_text.setString("X");
                        question_text.setCharacterSize(72);
                        const sf::FloatRect tb = question_text.getLocalBounds();
                        question_text.setOrigin({tb.position.x + tb.size.x / 2.f,
                                                 tb.position.y + tb.size.y / 2.f});
                        current_game_state = game_state_t::no_entries_enabled;
                        for (auto &shape : button_shapes) {
                            shape.setFillColor(color_disabled_toggle);
                        }
                        for (auto &text : answer_texts) {
                            text.setString("");
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
                        question_text.setCharacterSize(48);
                        question_text.setString(core::string::to_sfml_string(
                            is_hangul ? correct_entry.hangul : correct_entry.latin));
                        const sf::FloatRect tb = question_text.getLocalBounds();
                        question_text.setOrigin({tb.position.x + tb.size.x / 2.f,
                                                 tb.position.y + tb.size.y / 2.f});
                        memo_text.setString("");
                        for (std::size_t i = 0; i < 4; ++i) {
                            button_shapes[i].setFillColor(color_default_button);
                            answer_texts[i].setString(core::string::to_sfml_string(
                                is_hangul ? opts[i].latin : opts[i].hangul));
                            const sf::FloatRect ans_bounds = answer_texts[i].getLocalBounds();
                            answer_texts[i].setOrigin({ans_bounds.position.x + ans_bounds.size.x / 2.f,
                                                       ans_bounds.position.y + ans_bounds.size.y / 2.f});
                            core::text::set_integer_position(answer_texts[i], button_shapes[i].getPosition());
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
        window.clear(color_background);

        window.draw(question_circle);
        window.draw(question_text);
        if (current_game_state == game_state_t::show_result) {
            window.draw(memo_text);
        }
        for (std::size_t i = 0; i < 4; ++i) {
            window.draw(button_shapes[i]);
            window.draw(answer_texts[i]);
        }
        window.draw(percentage_text);
        for (std::size_t i = 0; i < toggle_buttons.size(); ++i) {
            window.draw(toggle_buttons[i]);
            window.draw(toggle_texts[i]);
        }

        // Display the newly drawn frame
        window.display();
    }
}

}  // namespace app
