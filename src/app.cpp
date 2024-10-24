/**
 * @file app.cpp
 */

#include <cstddef>        // for std::size_t
#include <string>         // for std::string
#include <unordered_map>  // for std::unordered_map
#include <vector>         // for std::vector

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <fmt/core.h>

#include "app.hpp"
#include "core/assets.hpp"
#include "core/colors.hpp"
#include "core/rng.hpp"
#include "core/string.hpp"
#include "modules/vocabulary.hpp"
#include "version.hpp"

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
 */
[[nodiscard]] sf::ContextSettings get_improved_context_settings(const unsigned int antialiasing = 8)
{
    sf::ContextSettings settings;
    settings.antialiasingLevel = antialiasing;
    return settings;
}

/**
 * @brief Private helper class that handles the user interface.
 *
 * On construction, the class sets up the window and initializes UI elements.
 * To run the application, call the "run()" method.
 *
 * @note This class is marked as `final` to prevent inheritance.
 */
class UI final {
  public:
    /**
     * @brief Construct a new UI object.
     */
    explicit UI()
        : window_(sf::VideoMode(800, 600),
                  fmt::format("aegyo ({})", PROJECT_VERSION),
                  sf::Style::Titlebar | sf::Style::Close,
                  // Overwrite the default context settings with improved settings
                  get_improved_context_settings()),
          font_(core::assets::load_font()),
          vocabulary_(),
          toggle_labels_({"Vow", "Con", "DCon", "CompV"}),
          toggle_categories_({modules::vocabulary::Category::BasicVowel,
                              modules::vocabulary::Category::BasicConsonant,
                              modules::vocabulary::Category::DoubleConsonant,
                              modules::vocabulary::Category::CompoundVowel}),
          toggle_states_({{modules::vocabulary::Category::BasicVowel, true},
                          {modules::vocabulary::Category::BasicConsonant, true},
                          {modules::vocabulary::Category::DoubleConsonant, true},
                          {modules::vocabulary::Category::CompoundVowel, true}})
    {
        // Enable V-Sync to limit the frame rate to the refresh rate of the monitor
        this->window_.setVerticalSyncEnabled(true);

        // Disable key repeat, as we only want one key press to register
        this->window_.setKeyRepeatEnabled(false);

        // Log anti-aliasing level
        fmt::print("Anti-aliasing level: {}\n", this->window_.getSettings().antialiasingLevel);
    }

    /**
     * @brief Run the main application loop.
     */
    void run()
    {
        // Member variables used within run()
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

        // UI Elements
        sf::CircleShape question_circle;
        sf::Text question_text;
        sf::Text memo_text;
        sf::Text percentage_text;

        std::vector<sf::CircleShape> button_shapes(4);
        std::vector<sf::Text> answer_buttons(4);

        std::vector<sf::RectangleShape> toggle_buttons;
        std::vector<sf::Text> toggle_texts;

        // Initialize UI elements
        // Initialize question circle
        question_circle.setRadius(80.f);
        question_circle.setFillColor(core::colors::question_circle);
        question_circle.setOrigin(80.f, 80.f);
        question_circle.setPosition(400.f, 150.f);

        // Initialize question text
        question_text.setFont(this->font_);
        question_text.setCharacterSize(48);
        question_text.setFillColor(core::colors::text);

        // Initialize memo text
        memo_text.setFont(this->font_);
        memo_text.setCharacterSize(16);
        memo_text.setFillColor(core::colors::text);
        memo_text.setPosition(400.f, 270.f);  // Position below the question circle

        // Initialize answer buttons
        constexpr float button_radius = 60.f;
        for (std::size_t idx = 0; idx < 4; ++idx) {
            button_shapes[idx].setRadius(button_radius);
            button_shapes[idx].setFillColor(core::colors::default_button);
            button_shapes[idx].setOrigin(button_radius, button_radius);
            answer_buttons[idx].setFont(this->font_);
            answer_buttons[idx].setCharacterSize(28);
            answer_buttons[idx].setFillColor(core::colors::text);
        }
        button_shapes[0].setPosition(250.f, 350.f);
        button_shapes[1].setPosition(550.f, 350.f);
        button_shapes[2].setPosition(250.f, 500.f);
        button_shapes[3].setPosition(550.f, 500.f);

        // Initialize percentage text
        percentage_text.setFont(this->font_);
        percentage_text.setCharacterSize(18);  // Smaller font size
        percentage_text.setFillColor(core::colors::text);
        percentage_text.setPosition(10.f, 10.f);  // Top-left corner

        // Initialize toggle buttons
        const float total_toggle_width = static_cast<float>(this->toggle_labels_.size()) * 60.f;
        const float start_x = static_cast<float>(this->window_.getSize().x) - total_toggle_width - 10.f;  // 10.f padding from the right

        for (std::size_t idx = 0; idx < this->toggle_categories_.size(); ++idx) {
            sf::RectangleShape button;
            button.setSize({50.f, 35.f});
            // Set fill color based on initial state
            if (this->toggle_states_.at(this->toggle_categories_[idx])) {
                button.setFillColor(core::colors::enabled_color);  // Enabled state color
            }
            else {
                button.setFillColor(core::colors::disabled_color);  // Disabled state color
            }
            button.setOutlineColor(core::colors::text);
            button.setOutlineThickness(1.f);
            button.setPosition(start_x + static_cast<float>(idx) * 60.f, 10.f);  // Positioned in the top-right corner

            toggle_buttons.emplace_back(button);

            sf::Text text;
            text.setFont(this->font_);
            text.setCharacterSize(14);
            text.setFillColor(core::colors::text);
            text.setString(this->toggle_labels_[idx]);
            // Center text in the button
            const sf::FloatRect text_bounds = text.getLocalBounds();
            text.setOrigin(text_bounds.left + text_bounds.width / 2.0f,
                           text_bounds.top + text_bounds.height / 2.0f);
            text.setPosition(button.getPosition() + sf::Vector2f(25.f, 17.5f));

            toggle_texts.emplace_back(text);
        }

        // Lambda functions for setup
        auto setup_new_question = [&]() {
            const auto optional_entry = this->vocabulary_.get_random_enabled_entry();
            if (!optional_entry.has_value()) {
                question_text.setString("X");
                question_text.setCharacterSize(72);  // Increase font size for the 'X'
                // Center text in the question circle
                const sf::FloatRect text_bounds = question_text.getLocalBounds();
                question_text.setOrigin(text_bounds.left + text_bounds.width / 2.0f,
                                        text_bounds.top + text_bounds.height / 2.0f);
                question_text.setPosition(question_circle.getPosition());

                game_state = GameState::NoEntriesEnabled;
                // Disable answer buttons visually
                for (auto &button_shape : button_shapes) {
                    button_shape.setFillColor(core::colors::disabled_color);
                }
                for (auto &answer_button : answer_buttons) {
                    answer_button.setString("");
                }
                // Clear memo text
                memo_text.setString("");
            }
            else {
                correct_entry = optional_entry.value();
                game_state = GameState::WaitingForAnswer;

                // Randomly decide whether to display Hangul or Latin using a Bernoulli distribution
                is_hangul = core::rng::RNG::get_random_bool();

                // Get unique options
                const auto options = this->vocabulary_.generate_enabled_question_options(correct_entry);

                // Find the index of the correct answer after shuffling
                for (std::size_t idx = 0; idx < 4; ++idx) {
                    if (options[idx].hangul == correct_entry.hangul) {
                        correct_index = idx;
                        break;
                    }
                }

                // Setup the question text
                question_text.setCharacterSize(48);  // Reset to default size
                question_text.setString(core::string::to_sfml_string(is_hangul ? correct_entry.hangul : correct_entry.latin));
                // Center text in the question circle
                const sf::FloatRect text_bounds = question_text.getLocalBounds();
                question_text.setOrigin(text_bounds.left + text_bounds.width / 2.0f,
                                        text_bounds.top + text_bounds.height / 2.0f);
                question_text.setPosition(question_circle.getPosition());

                // Clear memo text
                memo_text.setString("");

                // Setup answer buttons
                for (std::size_t idx = 0; idx < 4; ++idx) {
                    button_shapes[idx].setFillColor(core::colors::default_button);  // Reset button colors
                    answer_buttons[idx].setString(core::string::to_sfml_string(is_hangul ? options[idx].latin : options[idx].hangul));

                    // Center text in the answer buttons
                    const sf::FloatRect ans_text_bounds = answer_buttons[idx].getLocalBounds();
                    answer_buttons[idx].setOrigin(ans_text_bounds.left + ans_text_bounds.width / 2.0f,
                                                  ans_text_bounds.top + ans_text_bounds.height / 2.0f);
                    answer_buttons[idx].setPosition(button_shapes[idx].getPosition());
                }
            }
        };

        auto update_percentage_text = [&]() {
            const float percentage = total_questions > 0 ? (static_cast<float>(correct_answers) / static_cast<float>(total_questions)) * 100.f : 0.f;
            const auto percentage_str = fmt::format("게임 점수: {:.1f}%", percentage);
            percentage_text.setString(core::string::to_sfml_string(percentage_str));
        };

        // Initial setup
        setup_new_question();
        update_percentage_text();

        while (this->window_.isOpen()) {
            const sf::Vector2i mouse_pos = sf::Mouse::getPosition(this->window_);

            sf::Event event;
            while (this->window_.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    this->window_.close();
                }

                // Handle toggle button clicks
                if (event.type == sf::Event::MouseButtonReleased) {
                    for (std::size_t idx = 0; idx < toggle_buttons.size(); ++idx) {
                        if (toggle_buttons[idx].getGlobalBounds().contains(static_cast<float>(mouse_pos.x), static_cast<float>(mouse_pos.y))) {
                            // Toggle the category
                            const bool current_state = this->toggle_states_[this->toggle_categories_[idx]];
                            this->toggle_states_[this->toggle_categories_[idx]] = !current_state;
                            this->vocabulary_.set_category_enabled(this->toggle_categories_[idx], !current_state);
                            // Update button appearance
                            if (this->toggle_states_[this->toggle_categories_[idx]]) {
                                toggle_buttons[idx].setFillColor(core::colors::enabled_color);  // Enabled state color
                            }
                            else {
                                toggle_buttons[idx].setFillColor(core::colors::disabled_color);  // Disabled state color
                            }
                            // Reset the game
                            setup_new_question();
                            break;
                        }
                    }
                }

                if (game_state == GameState::WaitingForAnswer) {
                    if (event.type == sf::Event::MouseMoved) {
                        // Handle hover effect for answer buttons
                        for (std::size_t idx = 0; idx < 4; ++idx) {
                            if (button_shapes[idx].getGlobalBounds().contains(static_cast<float>(mouse_pos.x), static_cast<float>(mouse_pos.y))) {
                                button_shapes[idx].setFillColor(core::colors::hover_button);
                            }
                            else {
                                button_shapes[idx].setFillColor(core::colors::default_button);
                            }
                        }
                    }
                    else if (event.type == sf::Event::MouseButtonReleased) {
                        // Handle answer button clicks
                        for (std::size_t idx = 0; idx < 4; ++idx) {
                            if (button_shapes[idx].getGlobalBounds().contains(static_cast<float>(mouse_pos.x), static_cast<float>(mouse_pos.y))) {
                                game_state = GameState::ShowResult;
                                ++total_questions;
                                // Highlight the buttons based on the user's selection
                                if (idx == correct_index) {
                                    ++correct_answers;
                                    button_shapes[idx].setFillColor(core::colors::correct_answer);  // Correct answer
                                }
                                else {
                                    button_shapes[idx].setFillColor(core::colors::selected_wrong_answer);     // Chosen wrong answer (orange)
                                    button_shapes[correct_index].setFillColor(core::colors::correct_answer);  // Highlight correct answer
                                }
                                // Set the other buttons to incorrect color
                                for (std::size_t jdx = 0; jdx < 4; ++jdx) {
                                    if (jdx != idx && jdx != correct_index) {
                                        button_shapes[jdx].setFillColor(core::colors::incorrect_answer);  // Incorrect answers
                                    }
                                }
                                update_percentage_text();
                                // Display memo text
                                memo_text.setString(core::string::to_sfml_string(correct_entry.memo));
                                // Center memo text below the question circle
                                const sf::FloatRect memo_bounds = memo_text.getLocalBounds();
                                memo_text.setOrigin(memo_bounds.left + memo_bounds.width / 2.0f,
                                                    memo_bounds.top + memo_bounds.height / 2.0f);
                                memo_text.setPosition(400.f, 265.f);  // Adjust position as needed
                                break;
                            }
                        }
                    }
                    else if (event.type == sf::Event::KeyPressed) {
                        // Handle keyboard input
                        std::size_t selected_index = static_cast<std::size_t>(-1);
                        if (event.key.code == sf::Keyboard::Num1) {
                            selected_index = 0;
                        }
                        else if (event.key.code == sf::Keyboard::Num2) {
                            selected_index = 1;
                        }
                        else if (event.key.code == sf::Keyboard::Num3) {
                            selected_index = 2;
                        }
                        else if (event.key.code == sf::Keyboard::Num4) {
                            selected_index = 3;
                        }
                        if (selected_index != static_cast<std::size_t>(-1)) {
                            game_state = GameState::ShowResult;
                            ++total_questions;
                            // Highlight the buttons based on the user's selection
                            if (selected_index == correct_index) {
                                ++correct_answers;
                                button_shapes[selected_index].setFillColor(core::colors::correct_answer);  // Correct answer
                            }
                            else {
                                button_shapes[selected_index].setFillColor(core::colors::selected_wrong_answer);  // Chosen wrong answer (orange)
                                button_shapes[correct_index].setFillColor(core::colors::correct_answer);          // Highlight correct answer
                            }
                            // Set the other buttons to incorrect color
                            for (std::size_t jdx = 0; jdx < 4; ++jdx) {
                                if (jdx != selected_index && jdx != correct_index) {
                                    button_shapes[jdx].setFillColor(core::colors::incorrect_answer);  // Incorrect answers
                                }
                            }
                            update_percentage_text();
                            // Display memo text
                            memo_text.setString(core::string::to_sfml_string(correct_entry.memo));
                            // Center memo text below the question circle
                            const sf::FloatRect memo_bounds = memo_text.getLocalBounds();
                            memo_text.setOrigin(memo_bounds.left + memo_bounds.width / 2.0f,
                                                memo_bounds.top + memo_bounds.height / 2.0f);
                            memo_text.setPosition(400.f, 270.f);  // Adjust position as needed
                        }
                    }
                }
                else if (game_state == GameState::ShowResult) {
                    if (event.type == sf::Event::MouseButtonReleased || event.type == sf::Event::KeyPressed) {
                        // Proceed to the next question
                        setup_new_question();
                    }
                }

                // Handle hover effect for toggle buttons
                if (event.type == sf::Event::MouseMoved) {
                    for (std::size_t idx = 0; idx < toggle_buttons.size(); ++idx) {
                        if (toggle_buttons[idx].getGlobalBounds().contains(static_cast<float>(mouse_pos.x), static_cast<float>(mouse_pos.y))) {
                            toggle_buttons[idx].setOutlineThickness(2.f);
                        }
                        else {
                            toggle_buttons[idx].setOutlineThickness(1.f);
                        }
                    }
                }
            }

            // Handle hover effect when moving the mouse without any events
            if (game_state == GameState::WaitingForAnswer) {
                for (std::size_t idx = 0; idx < 4; ++idx) {
                    if (button_shapes[idx].getGlobalBounds().contains(static_cast<float>(mouse_pos.x), static_cast<float>(mouse_pos.y))) {
                        button_shapes[idx].setFillColor(core::colors::hover_button);
                    }
                    else {
                        button_shapes[idx].setFillColor(core::colors::default_button);
                    }
                }
            }
            else if (game_state == GameState::NoEntriesEnabled) {
                // Disable hover effect on answer buttons
                for (auto &button_shape : button_shapes) {
                    button_shape.setFillColor(core::colors::disabled_color);
                }
            }

            this->window_.clear(core::colors::background);  // Dark background color
            this->window_.draw(question_circle);
            this->window_.draw(question_text);
            // Draw memo text if in ShowResult state
            if (game_state == GameState::ShowResult) {
                this->window_.draw(memo_text);
            }
            for (std::size_t idx = 0; idx < 4; ++idx) {
                this->window_.draw(button_shapes[idx]);
                this->window_.draw(answer_buttons[idx]);
            }
            this->window_.draw(percentage_text);
            // Draw toggle buttons
            for (std::size_t idx = 0; idx < toggle_buttons.size(); ++idx) {
                this->window_.draw(toggle_buttons[idx]);
                this->window_.draw(toggle_texts[idx]);
            }
            this->window_.display();
        }
    }

  private:
    // Member variables
    sf::RenderWindow window_;
    const sf::Font &font_;
    modules::vocabulary::Vocabulary vocabulary_;

    // Toggle button states
    std::vector<std::string> toggle_labels_;
    std::vector<modules::vocabulary::Category> toggle_categories_;
    std::unordered_map<modules::vocabulary::Category, bool> toggle_states_;
};

}  // namespace

void run()
{
    UI().run();
}

}  // namespace app
