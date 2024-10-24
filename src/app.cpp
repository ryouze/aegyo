/**
 * @file app.cpp
 */

#include <array>          // for std::array
#include <cstddef>        // for std::size_t
#include <string>         // for std::string
#include <unordered_map>  // for std::unordered_map

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
                          {modules::vocabulary::Category::CompoundVowel, true}}),
          button_shapes_(),
          answer_buttons_()
    {
        // Enable V-Sync to limit the frame rate to the refresh rate of the monitor
        this->window_.setVerticalSyncEnabled(true);

        // Disable key repeat, as we only want one key press to register
        this->window_.setKeyRepeatEnabled(false);

        // Log anti-aliasing level
        fmt::print("Anti-aliasing level: {}\n", this->window_.getSettings().antialiasingLevel);

        // Initialize UI elements
        // Initialize question circle
        this->question_circle_.setRadius(80.f);
        this->question_circle_.setFillColor(core::colors::question_circle);
        this->question_circle_.setOrigin(80.f, 80.f);
        this->question_circle_.setPosition(400.f, 150.f);

        // Initialize question text
        this->question_text_.setFont(this->font_);
        this->question_text_.setCharacterSize(48);
        this->question_text_.setFillColor(core::colors::text);
        this->question_text_.setPosition(this->question_circle_.getPosition());

        // Initialize memo text
        this->memo_text_.setFont(this->font_);
        this->memo_text_.setCharacterSize(16);
        this->memo_text_.setFillColor(core::colors::text);
        this->memo_text_.setPosition(400.f, 270.f);  // Position below the question circle

        // Initialize answer buttons
        constexpr float button_radius = 60.f;
        for (std::size_t idx = 0; idx < 4; ++idx) {
            this->button_shapes_[idx].setRadius(button_radius);
            this->button_shapes_[idx].setFillColor(core::colors::default_button);
            this->button_shapes_[idx].setOrigin(button_radius, button_radius);
            this->answer_buttons_[idx].setFont(this->font_);
            this->answer_buttons_[idx].setCharacterSize(28);
            this->answer_buttons_[idx].setFillColor(core::colors::text);
        }
        this->button_shapes_[0].setPosition(250.f, 350.f);
        this->button_shapes_[1].setPosition(550.f, 350.f);
        this->button_shapes_[2].setPosition(250.f, 500.f);
        this->button_shapes_[3].setPosition(550.f, 500.f);

        // Set positions of answer button texts
        for (std::size_t idx = 0; idx < 4; ++idx) {
            this->answer_buttons_[idx].setPosition(this->button_shapes_[idx].getPosition());
        }

        // Initialize percentage text
        this->percentage_text_.setFont(this->font_);
        this->percentage_text_.setCharacterSize(18);  // Smaller font size
        this->percentage_text_.setFillColor(core::colors::text);
        this->percentage_text_.setPosition(10.f, 10.f);  // Top-left corner

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

            this->toggle_buttons_.emplace_back(button);

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

            this->toggle_texts_.emplace_back(text);
        }
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

        // Initial setup
        const auto update_percentage_text = [&]() {
            const float percentage = total_questions > 0 ? (static_cast<float>(correct_answers) / total_questions) * 100.f : 0.f;
            const auto percentage_str = fmt::format("게임 점수: {:.1f}%", percentage);
            this->percentage_text_.setString(core::string::to_sfml_string(percentage_str));
        };

        update_percentage_text();

        const auto setup_new_question = [&]() {
            const auto optional_entry = this->vocabulary_.get_random_enabled_entry();
            if (!optional_entry.has_value()) {
                this->question_text_.setString("X");
                this->question_text_.setCharacterSize(72);  // Increase font size for the 'X'
                // Center text in the question circle
                const sf::FloatRect text_bounds = this->question_text_.getLocalBounds();
                this->question_text_.setOrigin(text_bounds.left + text_bounds.width / 2.0f,
                                               text_bounds.top + text_bounds.height / 2.0f);

                game_state = GameState::NoEntriesEnabled;
                // Disable answer buttons visually
                for (auto &button_shape : this->button_shapes_) {
                    button_shape.setFillColor(core::colors::disabled_color);
                }
                for (auto &answer_button : this->answer_buttons_) {
                    answer_button.setString("");
                }
                // Clear memo text
                this->memo_text_.setString("");
            }
            else {
                correct_entry = optional_entry.value();

                is_hangul = core::rng::RNG::get_random_bool();

                const auto options = this->vocabulary_.generate_enabled_question_options(correct_entry);

                for (std::size_t idx = 0; idx < 4; ++idx) {
                    if (options[idx].hangul == correct_entry.hangul) {
                        correct_index = idx;
                        break;
                    }
                }

                this->question_text_.setCharacterSize(48);  // Reset to default size
                this->question_text_.setString(core::string::to_sfml_string(is_hangul ? correct_entry.hangul : correct_entry.latin));
                // Center text in the question circle
                const sf::FloatRect text_bounds = this->question_text_.getLocalBounds();
                this->question_text_.setOrigin(text_bounds.left + text_bounds.width / 2.0f,
                                               text_bounds.top + text_bounds.height / 2.0f);

                // Clear memo text
                this->memo_text_.setString("");

                // Setup answer buttons
                for (std::size_t idx = 0; idx < 4; ++idx) {
                    this->button_shapes_[idx].setFillColor(core::colors::default_button);  // Reset button colors
                    this->answer_buttons_[idx].setString(core::string::to_sfml_string(is_hangul ? options[idx].latin : options[idx].hangul));

                    // Center text in the answer buttons
                    const sf::FloatRect ans_text_bounds = this->answer_buttons_[idx].getLocalBounds();
                    this->answer_buttons_[idx].setOrigin(ans_text_bounds.left + ans_text_bounds.width / 2.0f,
                                                         ans_text_bounds.top + ans_text_bounds.height / 2.0f);
                    // Set position to center the text within the button
                    this->answer_buttons_[idx].setPosition(this->button_shapes_[idx].getPosition());
                }

                game_state = GameState::WaitingForAnswer;
            }
        };

        setup_new_question();

        // Main loop
        while (this->window_.isOpen()) {
            // Variables for event handling
            const sf::Vector2i mouse_pos = sf::Mouse::getPosition(this->window_);

            sf::Event event;
            while (this->window_.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    this->window_.close();
                }

                // Handle toggle button clicks
                if (event.type == sf::Event::MouseButtonReleased) {
                    for (std::size_t idx = 0; idx < this->toggle_buttons_.size(); ++idx) {
                        if (this->toggle_buttons_[idx].getGlobalBounds().contains(static_cast<float>(mouse_pos.x), static_cast<float>(mouse_pos.y))) {
                            // Toggle the category
                            const bool current_state = this->toggle_states_[this->toggle_categories_[idx]];
                            this->toggle_states_[this->toggle_categories_[idx]] = !current_state;
                            this->vocabulary_.set_category_enabled(this->toggle_categories_[idx], !current_state);
                            // Update button appearance
                            if (this->toggle_states_[this->toggle_categories_[idx]]) {
                                this->toggle_buttons_[idx].setFillColor(core::colors::enabled_color);  // Enabled state color
                            }
                            else {
                                this->toggle_buttons_[idx].setFillColor(core::colors::disabled_color);  // Disabled state color
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
                    for (std::size_t idx = 0; idx < this->toggle_buttons_.size(); ++idx) {
                        if (this->toggle_buttons_[idx].getGlobalBounds().contains(static_cast<float>(mouse_pos.x), static_cast<float>(mouse_pos.y))) {
                            this->toggle_buttons_[idx].setOutlineThickness(2.f);
                        }
                        else {
                            this->toggle_buttons_[idx].setOutlineThickness(1.f);
                        }
                    }
                }

                // Game logic
                if (game_state == GameState::WaitingForAnswer) {
                    if (event.type == sf::Event::MouseMoved) {
                        // Handle hover effect for answer buttons
                        for (std::size_t idx = 0; idx < 4; ++idx) {
                            if (this->button_shapes_[idx].getGlobalBounds().contains(static_cast<float>(mouse_pos.x), static_cast<float>(mouse_pos.y))) {
                                this->button_shapes_[idx].setFillColor(core::colors::hover_button);
                            }
                            else {
                                this->button_shapes_[idx].setFillColor(core::colors::default_button);
                            }
                        }
                    }
                    else if (event.type == sf::Event::MouseButtonReleased) {
                        // Handle answer button clicks
                        for (std::size_t idx = 0; idx < 4; ++idx) {
                            if (this->button_shapes_[idx].getGlobalBounds().contains(static_cast<float>(mouse_pos.x), static_cast<float>(mouse_pos.y))) {
                                ++total_questions;
                                if (idx == correct_index) {
                                    ++correct_answers;
                                    this->button_shapes_[idx].setFillColor(core::colors::correct_answer);
                                }
                                else {
                                    this->button_shapes_[idx].setFillColor(core::colors::selected_wrong_answer);
                                    this->button_shapes_[correct_index].setFillColor(core::colors::correct_answer);
                                }
                                for (std::size_t jdx = 0; jdx < 4; ++jdx) {
                                    if (jdx != idx && jdx != correct_index) {
                                        this->button_shapes_[jdx].setFillColor(core::colors::incorrect_answer);
                                    }
                                }
                                update_percentage_text();
                                // Display memo text
                                this->memo_text_.setString(core::string::to_sfml_string(correct_entry.memo));
                                // Center memo text
                                const sf::FloatRect memo_bounds = this->memo_text_.getLocalBounds();
                                this->memo_text_.setOrigin(memo_bounds.left + memo_bounds.width / 2.0f,
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
                                this->button_shapes_[selected_index].setFillColor(core::colors::correct_answer);
                            }
                            else {
                                this->button_shapes_[selected_index].setFillColor(core::colors::selected_wrong_answer);
                                this->button_shapes_[correct_index].setFillColor(core::colors::correct_answer);
                            }
                            for (std::size_t jdx = 0; jdx < 4; ++jdx) {
                                if (jdx != selected_index && jdx != correct_index) {
                                    this->button_shapes_[jdx].setFillColor(core::colors::incorrect_answer);
                                }
                            }
                            update_percentage_text();
                            // Display memo text
                            this->memo_text_.setString(core::string::to_sfml_string(correct_entry.memo));
                            // Center memo text
                            const sf::FloatRect memo_bounds = this->memo_text_.getLocalBounds();
                            this->memo_text_.setOrigin(memo_bounds.left + memo_bounds.width / 2.0f,
                                                       memo_bounds.top + memo_bounds.height / 2.0f);
                            game_state = GameState::ShowResult;
                        }
                    }
                }
                else if (game_state == GameState::ShowResult) {
                    if (event.type == sf::Event::MouseButtonReleased || event.type == sf::Event::KeyPressed) {
                        // Proceed to the next question
                        // Hide memo text
                        this->memo_text_.setString("");
                        setup_new_question();
                    }
                }
                else if (game_state == GameState::NoEntriesEnabled) {
                    // Do nothing; wait for user to toggle categories
                }
            }

            // Render
            this->window_.clear(core::colors::background);
            this->window_.draw(this->question_circle_);
            this->window_.draw(this->question_text_);
            if (game_state == GameState::ShowResult) {
                this->window_.draw(this->memo_text_);
            }
            for (std::size_t idx = 0; idx < 4; ++idx) {
                this->window_.draw(this->button_shapes_[idx]);
                this->window_.draw(this->answer_buttons_[idx]);
            }
            this->window_.draw(this->percentage_text_);
            for (std::size_t idx = 0; idx < this->toggle_buttons_.size(); ++idx) {
                this->window_.draw(this->toggle_buttons_[idx]);
                this->window_.draw(this->toggle_texts_[idx]);
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
    std::array<std::string, 4> toggle_labels_;
    std::array<modules::vocabulary::Category, 4> toggle_categories_;
    std::unordered_map<modules::vocabulary::Category, bool> toggle_states_;

    // UI Elements
    sf::CircleShape question_circle_;
    sf::Text question_text_;
    sf::Text memo_text_;
    sf::Text percentage_text_;

    std::array<sf::CircleShape, 4> button_shapes_;
    std::array<sf::Text, 4> answer_buttons_;

    std::vector<sf::RectangleShape> toggle_buttons_;
    std::vector<sf::Text> toggle_texts_;
};

}  // namespace

void run()
{
    UI().run();
}

}  // namespace app
