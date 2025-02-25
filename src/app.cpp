/**
 * @file app.cpp
 */

#include <array>          // for std::array
#include <cstddef>        // for std::size_t
#include <memory>         // for std::unique_ptr
#include <optional>       // for std::optional
#include <string>         // for std::string
#include <unordered_map>  // for std::unordered_map
#include <vector>         // for std::vector

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <fmt/core.h>

#include "app.hpp"
#include "core/graphics/font.hpp"
#include "core/math/rng.hpp"
#include "core/settings/antialiasing.hpp"
#include "core/settings/colors.hpp"
#include "core/settings/screen.hpp"
#include "modules/vocabulary.hpp"
#include "ui/circles.hpp"
#include "ui/widgets.hpp"
#include "version.hpp"
#if defined(_WIN32)
#include "core/platform/windows.cpp"
#endif

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
    sf::RenderWindow window(
        sf::VideoMode({core::settings::screen::WIDTH, core::settings::screen::HEIGHT}),
        fmt::format("aegyo ({})", PROJECT_VERSION),
        sf::Style::Titlebar | sf::Style::Close,  // No resize, no fullscreen (too much work to handle, especially on macOS)
        sf::State::Windowed,
        core::settings::antialiasing::get_improved_context_settings());  // Enable anti-aliasing

    // Enable V-Sync to limit the frame rate to the refresh rate of the monitor
    window.setVerticalSyncEnabled(true);

    // Disable key repeat, as we only want one key press to register
    window.setKeyRepeatEnabled(false);

#if defined(_WIN32)
    // Set window titlebar icon (Windows-only)
    // macOS doesn't have titlebar icons, GNU/Linux is DE-dependent
    if (const auto e = core::platform::windows::set_titlebar_icon(window); e.has_value()) {
        fmt::print(stderr, "Warning: {}\n", *e);
    }
#endif

    // Request focus on the window
    window.requestFocus();

    // Load embedded NanumGothic font
    std::unique_ptr<sf::Font> font = core::graphics::font::load();  // Ownership is transferred here

    // Prepare vocabulary and interface items
    modules::vocabulary::Vocabulary vocabulary_obj;
    std::unordered_map<modules::vocabulary::Category, bool> toggle_states = {
        {modules::vocabulary::Category::BasicVowel, true},
        {modules::vocabulary::Category::BasicConsonant, true},
        {modules::vocabulary::Category::DoubleConsonant, true},
        {modules::vocabulary::Category::CompoundVowel, true}};

    const std::array<std::string, 4> toggle_labels = {"Vow", "Con", "DCon", "CompV"};
    const std::array<modules::vocabulary::Category, 4> toggle_categories = {
        modules::vocabulary::Category::BasicVowel,
        modules::vocabulary::Category::BasicConsonant,
        modules::vocabulary::Category::DoubleConsonant,
        modules::vocabulary::Category::CompoundVowel};

    std::array<sf::RectangleShape, 4> toggle_buttons;
    std::array<ui::components::base::Text, 4> toggle_texts = {
        ui::components::base::Text(*font),
        ui::components::base::Text(*font),
        ui::components::base::Text(*font),
        ui::components::base::Text(*font)};
    {
        constexpr float total_toggle_width = 4.f * 60.f;
        const float start_x = static_cast<float>(window.getSize().x) - total_toggle_width - 10.f;
        for (std::size_t i = 0; i < 4; ++i) {
            sf::RectangleShape btn(sf::Vector2f(50.f, 35.f));
            btn.setOutlineColor(core::settings::colors::text::normal);
            btn.setOutlineThickness(1.f);
            btn.setPosition(sf::Vector2f(start_x + static_cast<float>(i) * 60.f, 10.f));
            if (toggle_states.at(toggle_categories[i])) {
                btn.setFillColor(core::settings::colors::category::enabled);
            }
            else {
                btn.setFillColor(core::settings::colors::category::disabled);
            }
            toggle_buttons[i] = btn;

            ui::components::base::Text lbl(*font);
            lbl.setCharacterSize(14);
            lbl.setFillColor(core::settings::colors::text::normal);
            lbl.setString(toggle_labels[i]);
            const sf::FloatRect b = lbl.getLocalBounds();
            lbl.setOrigin(sf::Vector2f(b.position.x + b.size.x / 2.f, b.position.y + b.size.y / 2.f));
            lbl.setPosition(btn.getPosition() + sf::Vector2f(25.f, 17.5f));
            toggle_texts[i] = lbl;
        }
    }

    ui::circles::Question question_circle(*font);
    ui::widgets::Memo memo_text(*font);
    ui::widgets::Percentage percentage_display(*font);
    std::array<ui::circles::Answer, 4> answer_circles{
        ui::circles::Answer(*font, ui::circles::AnswerPosition::TopLeft),
        ui::circles::Answer(*font, ui::circles::AnswerPosition::TopRight),
        ui::circles::Answer(*font, ui::circles::AnswerPosition::BottomLeft),
        ui::circles::Answer(*font, ui::circles::AnswerPosition::BottomRight)};

    GameState current_state = GameState::Waiting;

    modules::vocabulary::Entry correct_entry;
    std::size_t correct_index = 0;
    bool is_hangul = true;

    // Helper to start or reset a question
    const auto initialize_question = [&](const bool reset_score) {
        if (reset_score) {
            percentage_display.reset();
        }
        const std::optional<modules::vocabulary::Entry> maybe_entry =
            vocabulary_obj.get_random_enabled_entry(toggle_states);
        if (!maybe_entry.has_value()) {
            question_circle.set_invalid();
            current_state = GameState::NoEntries;
            for (auto &c : answer_circles) {
                c.set_invalid();
            }
            memo_text.hide();
            return;
        }
        correct_entry = maybe_entry.value();
        is_hangul = core::math::rng::get_random_bool();
        const std::vector<modules::vocabulary::Entry> opts =
            vocabulary_obj.generate_enabled_question_options(correct_entry, toggle_states);
        for (std::size_t i = 0; i < 4; ++i) {
            if (opts[i].hangul == correct_entry.hangul) {
                correct_index = i;
                break;
            }
        }
        question_circle.set_question(is_hangul ? correct_entry.hangul : correct_entry.latin);
        memo_text.hide();
        for (std::size_t i = 0; i < 4; ++i) {
            answer_circles[i].set_answer(is_hangul ? opts[i].latin : opts[i].hangul);
        }
        current_state = GameState::Waiting;
    };

    initialize_question(false);

    // Main loop
    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            else if (const sf::Event::MouseButtonReleased *mouseUp = event->getIf<sf::Event::MouseButtonReleased>()) {
                if (mouseUp->button == sf::Mouse::Button::Left) {
                    const sf::Vector2f pos(
                        static_cast<float>(mouseUp->position.x),
                        static_cast<float>(mouseUp->position.y));
                    // Handle toggles
                    for (std::size_t i = 0; i < toggle_buttons.size(); ++i) {
                        const sf::FloatRect tb = toggle_buttons[i].getGlobalBounds();
                        if (sf::FloatRect(tb.position, tb.size).contains(pos)) {
                            const bool old = toggle_states.at(toggle_categories[i]);
                            toggle_states[toggle_categories[i]] = !old;
                            if (toggle_states.at(toggle_categories[i])) {
                                toggle_buttons[i].setFillColor(core::settings::colors::category::enabled);
                            }
                            else {
                                toggle_buttons[i].setFillColor(core::settings::colors::category::disabled);
                            }
                            initialize_question(true);
                            break;
                        }
                    }
                    // Handle answers (Waiting state only)
                    if (current_state == GameState::Waiting) {
                        for (std::size_t i = 0; i < 4; ++i) {
                            if (answer_circles[i].is_hovering(pos)) {
                                if (i == correct_index) {
                                    percentage_display.add_correct_answer();
                                    answer_circles[i].set_correct_answer_highlight();
                                }
                                else {
                                    percentage_display.add_incorrect_answer();
                                    answer_circles[i].set_selected_wrong_answer_highlight();
                                    answer_circles[correct_index].set_correct_answer_highlight();
                                }
                                for (std::size_t j = 0; j < 4; ++j) {
                                    if (j != i && j != correct_index) {
                                        answer_circles[j].set_incorrect_answer_highlight();
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
                        memo_text.hide();
                        initialize_question(false);
                    }
                }
            }
            else if (const sf::Event::MouseMoved *mouseMove = event->getIf<sf::Event::MouseMoved>()) {
                // Toggle button hover
                for (std::size_t i = 0; i < toggle_buttons.size(); ++i) {
                    const sf::FloatRect tb = toggle_buttons[i].getGlobalBounds();
                    if (sf::FloatRect(tb.position, tb.size)
                            .contains(sf::Vector2f(static_cast<float>(mouseMove->position.x),
                                                   static_cast<float>(mouseMove->position.y)))) {
                        toggle_buttons[i].setOutlineThickness(2.f);
                    }
                    else {
                        toggle_buttons[i].setOutlineThickness(1.f);
                    }
                }
                // Answer circle hover
                if (current_state == GameState::Waiting) {
                    const sf::Vector2f pos(
                        static_cast<float>(mouseMove->position.x),
                        static_cast<float>(mouseMove->position.y));
                    for (auto &circle : answer_circles) {
                        circle.toggle_hover_highlight(pos);
                    }
                }
            }
            else if (const sf::Event::KeyPressed *keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (current_state == GameState::Waiting) {
                    std::optional<std::size_t> sel;
                    switch (keyPressed->scancode) {
                    case sf::Keyboard::Scancode::Num1:
                        sel = 0;
                        break;
                    case sf::Keyboard::Scancode::Num2:
                        sel = 1;
                        break;
                    case sf::Keyboard::Scancode::Num3:
                        sel = 2;
                        break;
                    case sf::Keyboard::Scancode::Num4:
                        sel = 3;
                        break;
                    default:
                        break;
                    }
                    if (sel.has_value()) {
                        if (*sel == correct_index) {
                            percentage_display.add_correct_answer();
                            answer_circles[*sel].set_correct_answer_highlight();
                        }
                        else {
                            percentage_display.add_incorrect_answer();
                            answer_circles[*sel].set_selected_wrong_answer_highlight();
                            answer_circles[correct_index].set_correct_answer_highlight();
                        }
                        for (std::size_t j = 0; j < 4; ++j) {
                            if (j != *sel && j != correct_index) {
                                answer_circles[j].set_incorrect_answer_highlight();
                            }
                        }
                        memo_text.set(correct_entry.memo);
                        current_state = GameState::ShowingResult;
                    }
                }
                else if (current_state == GameState::ShowingResult) {
                    memo_text.hide();
                    initialize_question(false);
                }
            }
        }

        // Draw everything
        window.clear(core::settings::colors::background::normal);
        question_circle.draw(window);
        if (current_state == GameState::ShowingResult) {
            memo_text.draw(window);
        }
        for (const auto &circle : answer_circles) {
            circle.draw(window);
        }
        percentage_display.draw(window);
        for (std::size_t i = 0; i < toggle_buttons.size(); ++i) {
            window.draw(toggle_buttons[i]);
            window.draw(toggle_texts[i]);
        }
        window.display();
    }
}

}  // namespace app
