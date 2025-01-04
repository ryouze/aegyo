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
#include "core/assets.hpp"
#include "core/rng.hpp"
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
    settings.antialiasingLevel = antialiasing;
    return settings;
}

}  // namespace

void run()
{
    // -------------------------------------------------------------------------
    // All color definitions (formerly static members of UI)
    // -------------------------------------------------------------------------
    const sf::Color colors_background = sf::Color(30, 30, 30);          // Background color
    const sf::Color colors_text = sf::Color(240, 240, 240);             // Text color
    const sf::Color colors_enabled_toggle = sf::Color(100, 200, 100);   // Green for enabled
    const sf::Color colors_disabled_toggle = sf::Color(200, 100, 100);  // Red for disabled
    // Gray for hover (not used in the toggles below, but left here to match the original code)
    // const sf::Color colors_hover_toggle = sf::Color(150, 150, 150);

    // Answer button colors
    const sf::Color colors_default_button = sf::Color(80, 80, 80);
    const sf::Color colors_hover_button = sf::Color(100, 100, 100);
    const sf::Color colors_correct_answer = sf::Color(30, 130, 30);
    const sf::Color colors_incorrect_answer = sf::Color(130, 30, 30);
    const sf::Color colors_selected_wrong_answer = sf::Color(200, 120, 0);

    // Question circle color
    const sf::Color colors_question_circle = sf::Color(50, 50, 50);

    // -------------------------------------------------------------------------
    // Window setup (moved from UI constructor)
    // -------------------------------------------------------------------------
    sf::RenderWindow window(
        sf::VideoMode(800, 600),
        fmt::format("aegyo ({})", PROJECT_VERSION),
        sf::Style::Titlebar | sf::Style::Close,
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

    // -------------------------------------------------------------------------
    // Font, vocabulary, and toggles (moved from UI)
    // -------------------------------------------------------------------------
    // The font is loaded from assets; this returns a reference.
    const sf::Font &font = core::assets::load_font();
    modules::vocabulary::Vocabulary vocabulary;

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

    // -------------------------------------------------------------------------
    // UI elements (moved from UI)
    // -------------------------------------------------------------------------
    // Question circle
    sf::CircleShape question_circle;
    question_circle.setRadius(80.f);
    question_circle.setPointCount(100);
    question_circle.setFillColor(colors_question_circle);
    question_circle.setOrigin(80.f, 80.f);
    question_circle.setPosition(400.f, 150.f);

    // Question text
    sf::Text question_text;
    question_text.setFont(font);
    question_text.setCharacterSize(48);
    question_text.setFillColor(colors_text);
    core::text::set_integer_position(question_text, question_circle.getPosition());

    // Memo text
    sf::Text memo_text;
    memo_text.setFont(font);
    memo_text.setCharacterSize(16);
    memo_text.setFillColor(colors_text);
    core::text::set_integer_position(memo_text, 400.f, 270.f);  // Position below the question circle

    // Answer buttons
    std::array<sf::CircleShape, 4> button_shapes;
    std::array<sf::Text, 4> answer_buttons;
    constexpr float button_radius = 60.f;
    for (std::size_t idx = 0; idx < 4; ++idx) {
        button_shapes[idx].setRadius(button_radius);
        button_shapes[idx].setPointCount(100);
        button_shapes[idx].setFillColor(colors_default_button);
        button_shapes[idx].setOrigin(button_radius, button_radius);

        answer_buttons[idx].setFont(font);
        answer_buttons[idx].setCharacterSize(28);
        answer_buttons[idx].setFillColor(colors_text);
    }
    button_shapes[0].setPosition(250.f, 350.f);
    button_shapes[1].setPosition(550.f, 350.f);
    button_shapes[2].setPosition(250.f, 500.f);
    button_shapes[3].setPosition(550.f, 500.f);

    // Center answer button texts
    for (std::size_t idx = 0; idx < 4; ++idx) {
        core::text::set_integer_position(answer_buttons[idx], button_shapes[idx].getPosition());
    }

    // Percentage text
    sf::Text percentage_text;
    percentage_text.setFont(font);
    percentage_text.setCharacterSize(18);  // Smaller font size
    percentage_text.setFillColor(colors_text);
    core::text::set_integer_position(percentage_text, 10.f, 10.f);  // Top-left corner

    // Toggle buttons
    std::array<sf::RectangleShape, 4> toggle_buttons;
    std::array<sf::Text, 4> toggle_texts;
    constexpr float total_toggle_width = 4.f * 60.f;                                           // 4 buttons * 60 width
    const float start_x = static_cast<float>(window.getSize().x) - total_toggle_width - 10.f;  // 10.f padding from the right

    for (std::size_t idx = 0; idx < 4; ++idx) {
        sf::RectangleShape button;
        button.setSize({50.f, 35.f});
        // Set fill color based on initial state
        if (toggle_states.at(toggle_categories[idx])) {
            button.setFillColor(colors_enabled_toggle);  // Enabled state color
        }
        else {
            button.setFillColor(colors_disabled_toggle);  // Disabled state color
        }
        button.setOutlineColor(colors_text);
        button.setOutlineThickness(1.f);
        button.setPosition(start_x + static_cast<float>(idx) * 60.f, 10.f);

        toggle_buttons[idx] = button;

        sf::Text text;
        text.setFont(font);
        text.setCharacterSize(14);
        text.setFillColor(colors_text);
        text.setString(toggle_labels[idx]);

        // Center text in the button
        const sf::FloatRect text_bounds = text.getLocalBounds();
        text.setOrigin(text_bounds.left + text_bounds.width / 2.0f,
                       text_bounds.top + text_bounds.height / 2.0f);
        core::text::set_integer_position(text, button.getPosition() + sf::Vector2f(25.f, 17.5f));

        toggle_texts[idx] = text;
    }

    // -------------------------------------------------------------------------
    // Main loop logic (moved from UI::run())
    // -------------------------------------------------------------------------
    enum class GameState {
        WaitingForAnswer,
        ShowResult,
        NoEntriesEnabled
    };
    GameState game_state = GameState::WaitingForAnswer;

    modules::vocabulary::Entry correct_entry;
    std::size_t correct_index = 0;
    bool is_hangul = true;

    std::size_t total_questions = 0;
    std::size_t correct_answers = 0;

    // Lambda to update score percentage
    const auto update_percentage_text = [&]() {
        const float percentage =
            total_questions > 0
                ? (static_cast<float>(correct_answers) / static_cast<float>(total_questions)) * 100.f
                : 0.f;
        const auto percentage_str = fmt::format("게임 점수: {:.1f}%", percentage);
        percentage_text.setString(core::string::to_sfml_string(percentage_str));
    };

    update_percentage_text();

    // Lambda to set up a new question
    const auto setup_new_question = [&]() {
        const auto optional_entry = vocabulary.get_random_enabled_entry(toggle_states);

        // No entries enabled; display 'X' in the question circle
        if (!optional_entry.has_value()) {
            question_text.setString("X");
            question_text.setCharacterSize(72);  // Increase font size for the 'X'
            // Center text in the question circle
            const sf::FloatRect text_bounds = question_text.getLocalBounds();
            question_text.setOrigin(text_bounds.left + text_bounds.width / 2.0f,
                                    text_bounds.top + text_bounds.height / 2.0f);

            game_state = GameState::NoEntriesEnabled;
            // Disable answer buttons visually
            for (auto &button_shape : button_shapes) {
                button_shape.setFillColor(colors_disabled_toggle);
            }
            for (auto &answer_button : answer_buttons) {
                answer_button.setString("");
            }
            // Clear memo text
            memo_text.setString("");
        }
        else {
            // Entries are enabled; setup new question
            correct_entry = optional_entry.value();
            is_hangul = core::rng::RNG::get_random_bool();

            const auto options =
                vocabulary.generate_enabled_question_options(correct_entry, toggle_states);

            for (std::size_t idx = 0; idx < 4; ++idx) {
                if (options[idx].hangul == correct_entry.hangul) {
                    correct_index = idx;
                    break;
                }
            }

            question_text.setCharacterSize(48);  // Reset to default size
            question_text.setString(core::string::to_sfml_string(
                is_hangul ? correct_entry.hangul : correct_entry.latin));
            // Center text in the question circle
            const sf::FloatRect text_bounds = question_text.getLocalBounds();
            question_text.setOrigin(text_bounds.left + text_bounds.width / 2.0f,
                                    text_bounds.top + text_bounds.height / 2.0f);

            // Clear memo text
            memo_text.setString("");

            // Setup answer buttons
            for (std::size_t idx = 0; idx < 4; ++idx) {
                button_shapes[idx].setFillColor(colors_default_button);  // Reset button colors
                answer_buttons[idx].setString(core::string::to_sfml_string(
                    is_hangul ? options[idx].latin : options[idx].hangul));

                // Center text in the answer buttons
                const sf::FloatRect ans_text_bounds = answer_buttons[idx].getLocalBounds();
                answer_buttons[idx].setOrigin(ans_text_bounds.left + ans_text_bounds.width / 2.0f,
                                              ans_text_bounds.top + ans_text_bounds.height / 2.0f);
                // Set position to center the text within the button
                core::text::set_integer_position(answer_buttons[idx], button_shapes[idx].getPosition());
            }

            game_state = GameState::WaitingForAnswer;
        }
    };

    setup_new_question();

    // -------------------------------------------------------------------------
    // Main application loop
    // -------------------------------------------------------------------------
    while (window.isOpen()) {
        const sf::Vector2i mouse_pos = sf::Mouse::getPosition(window);
        sf::Event event;

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            // Handle toggle button clicks
            if (event.type == sf::Event::MouseButtonReleased) {
                for (std::size_t idx = 0; idx < toggle_buttons.size(); ++idx) {
                    if (toggle_buttons[idx].getGlobalBounds().contains(
                            static_cast<float>(mouse_pos.x), static_cast<float>(mouse_pos.y))) {
                        // Toggle the category
                        const bool current_state = toggle_states[toggle_categories[idx]];
                        toggle_states[toggle_categories[idx]] = !current_state;
                        // Update button appearance
                        if (toggle_states[toggle_categories[idx]]) {
                            toggle_buttons[idx].setFillColor(colors_enabled_toggle);  // Enabled
                        }
                        else {
                            toggle_buttons[idx].setFillColor(colors_disabled_toggle);  // Disabled
                        }
                        // Reset the game
                        total_questions = 0;
                        correct_answers = 0;
                        update_percentage_text();
                        setup_new_question();
                        break;
                    }
                }
            }

            // Handle hover effect for toggle buttons
            if (event.type == sf::Event::MouseMoved) {
                for (std::size_t idx = 0; idx < toggle_buttons.size(); ++idx) {
                    if (toggle_buttons[idx].getGlobalBounds().contains(
                            static_cast<float>(mouse_pos.x), static_cast<float>(mouse_pos.y))) {
                        toggle_buttons[idx].setOutlineThickness(2.f);
                    }
                    else {
                        toggle_buttons[idx].setOutlineThickness(1.f);
                    }
                }
            }

            // -----------------------------------------------------------------
            // Game logic
            // -----------------------------------------------------------------
            if (game_state == GameState::WaitingForAnswer) {
                if (event.type == sf::Event::MouseMoved) {
                    // Hover effect for answer buttons
                    for (std::size_t idx = 0; idx < 4; ++idx) {
                        if (button_shapes[idx].getGlobalBounds().contains(
                                static_cast<float>(mouse_pos.x), static_cast<float>(mouse_pos.y))) {
                            button_shapes[idx].setFillColor(colors_hover_button);
                        }
                        else {
                            button_shapes[idx].setFillColor(colors_default_button);
                        }
                    }
                }
                else if (event.type == sf::Event::MouseButtonReleased) {
                    // Answer button clicks
                    for (std::size_t idx = 0; idx < 4; ++idx) {
                        if (button_shapes[idx].getGlobalBounds().contains(
                                static_cast<float>(mouse_pos.x), static_cast<float>(mouse_pos.y))) {
                            ++total_questions;
                            if (idx == correct_index) {
                                ++correct_answers;
                                button_shapes[idx].setFillColor(colors_correct_answer);
                            }
                            else {
                                button_shapes[idx].setFillColor(colors_selected_wrong_answer);
                                button_shapes[correct_index].setFillColor(colors_correct_answer);
                            }
                            for (std::size_t jdx = 0; jdx < 4; ++jdx) {
                                if (jdx != idx && jdx != correct_index) {
                                    button_shapes[jdx].setFillColor(colors_incorrect_answer);
                                }
                            }
                            update_percentage_text();
                            // Display memo text
                            memo_text.setString(core::string::to_sfml_string(correct_entry.memo));
                            // Center memo text
                            const sf::FloatRect memo_bounds = memo_text.getLocalBounds();
                            memo_text.setOrigin(memo_bounds.left + memo_bounds.width / 2.0f,
                                                memo_bounds.top + memo_bounds.height / 2.0f);
                            game_state = GameState::ShowResult;
                            break;
                        }
                    }
                }
                else if (event.type == sf::Event::KeyPressed) {
                    // Handle keyboard input
                    const auto key_code = event.key.code;
                    std::size_t selected_index = std::numeric_limits<std::size_t>::max();
                    switch (key_code) {
                    case sf::Keyboard::Num1:
                        selected_index = 0;
                        break;
                    case sf::Keyboard::Num2:
                        selected_index = 1;
                        break;
                    case sf::Keyboard::Num3:
                        selected_index = 2;
                        break;
                    case sf::Keyboard::Num4:
                        selected_index = 3;
                        break;
                    default:
                        break;
                    }
                    if (selected_index < 4) {
                        ++total_questions;
                        if (selected_index == correct_index) {
                            ++correct_answers;
                            button_shapes[selected_index].setFillColor(colors_correct_answer);
                        }
                        else {
                            button_shapes[selected_index].setFillColor(colors_selected_wrong_answer);
                            button_shapes[correct_index].setFillColor(colors_correct_answer);
                        }
                        for (std::size_t jdx = 0; jdx < 4; ++jdx) {
                            if (jdx != selected_index && jdx != correct_index) {
                                button_shapes[jdx].setFillColor(colors_incorrect_answer);
                            }
                        }
                        update_percentage_text();
                        // Display memo text
                        memo_text.setString(core::string::to_sfml_string(correct_entry.memo));
                        // Center memo text
                        const sf::FloatRect memo_bounds = memo_text.getLocalBounds();
                        memo_text.setOrigin(memo_bounds.left + memo_bounds.width / 2.0f,
                                            memo_bounds.top + memo_bounds.height / 2.0f);
                        game_state = GameState::ShowResult;
                    }
                }
            }
            else if (game_state == GameState::ShowResult) {
                if (event.type == sf::Event::MouseButtonReleased ||
                    event.type == sf::Event::KeyPressed) {
                    // Proceed to the next question
                    memo_text.setString("");  // Hide memo text
                    setup_new_question();
                }
            }
            else if (game_state == GameState::NoEntriesEnabled) {
                // Do nothing; wait for user to toggle categories
            }
        }

        // ---------------------------------------------------------------------
        // Render
        // ---------------------------------------------------------------------
        window.clear(colors_background);

        window.draw(question_circle);
        window.draw(question_text);

        if (game_state == GameState::ShowResult) {
            window.draw(memo_text);
        }

        for (std::size_t idx = 0; idx < 4; ++idx) {
            window.draw(button_shapes[idx]);
            window.draw(answer_buttons[idx]);
        }
        window.draw(percentage_text);

        for (std::size_t idx = 0; idx < toggle_buttons.size(); ++idx) {
            window.draw(toggle_buttons[idx]);
            window.draw(toggle_texts[idx]);
        }

        window.display();
    }
}

}  // namespace app
