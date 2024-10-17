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
#include "core/vocabulary.hpp"
#include "version.hpp"

namespace app {

namespace {

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
        : window_(sf::VideoMode(800, 600), fmt::format("aegyo ({})", PROJECT_VERSION), sf::Style::Titlebar | sf::Style::Close),
          font_(core::assets::load_font()),
          vocabulary_(),
          game_state_(GameState::WaitingForAnswer),
          correct_entry_(),
          answer_buttons_(4),
          correct_index_(0),
          is_hangul_(true),
          total_questions_(0),
          correct_answers_(0),
          question_circle_(),
          question_text_(),
          button_shapes_(4),
          percentage_text_(),
          toggle_labels_({"Vow", "Con", "DCon", "CompV"}),
          toggle_categories_({core::vocabulary::Category::BasicVowel,
                              core::vocabulary::Category::BasicConsonant,
                              core::vocabulary::Category::DoubleConsonant,
                              core::vocabulary::Category::CompoundVowel}),
          toggle_states_({{core::vocabulary::Category::BasicVowel, true},
                          {core::vocabulary::Category::BasicConsonant, true},
                          {core::vocabulary::Category::DoubleConsonant, true},
                          {core::vocabulary::Category::CompoundVowel, true}})
    {
        // Enable V-Sync to limit the frame rate to the refresh rate of the monitor
        this->window_.setVerticalSyncEnabled(true);

        // Disable key repeat, as we only want one key press to register
        this->window_.setKeyRepeatEnabled(false);

        // Initialize question circle
        this->question_circle_.setRadius(80.f);
        this->question_circle_.setFillColor(core::colors::question_circle);
        this->question_circle_.setOrigin(80.f, 80.f);
        this->question_circle_.setPosition(400.f, 150.f);

        // Initialize question text
        this->question_text_.setFont(this->font_);
        this->question_text_.setCharacterSize(48);
        this->question_text_.setFillColor(core::colors::text);

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

            this->toggle_buttons_.push_back(button);

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

            this->toggle_texts_.push_back(text);
        }
    }

    /**
     * @brief Run the main application loop.
     */
    void run()
    {
        // Lambda functions for setup
        auto setup_new_question = [&]() {
            this->correct_entry_ = this->vocabulary_.get_random_entry();
            if (this->correct_entry_.hangul == "N/A") {
                this->question_text_.setString("X");
                this->question_text_.setCharacterSize(72);  // Increase font size for the 'X'
                // Center text in the question circle
                const sf::FloatRect text_bounds = this->question_text_.getLocalBounds();
                this->question_text_.setOrigin(text_bounds.left + text_bounds.width / 2.0f,
                                               text_bounds.top + text_bounds.height / 2.0f);
                this->question_text_.setPosition(this->question_circle_.getPosition());

                this->game_state_ = GameState::NoEntriesEnabled;
                // Disable answer buttons visually
                for (auto &button_shape : this->button_shapes_) {
                    button_shape.setFillColor(core::colors::disabled_color);
                }
                for (auto &answer_button : this->answer_buttons_) {
                    answer_button.setString("");
                }
            }
            else {
                this->game_state_ = GameState::WaitingForAnswer;

                // Randomly decide whether to display Hangul or Latin using a Bernoulli distribution
                this->is_hangul_ = core::rng::RNG::get_random_bool();

                // Get unique options
                const auto options = this->vocabulary_.get_question_options(this->correct_entry_);

                // Find the index of the correct answer after shuffling
                for (std::size_t idx = 0; idx < options.size(); ++idx) {
                    if (options[idx].hangul == this->correct_entry_.hangul) {
                        this->correct_index_ = idx;
                        break;
                    }
                }

                // Setup the question text
                this->question_text_.setCharacterSize(48);  // Reset to default size
                this->question_text_.setString(core::string::to_sfml_string(this->is_hangul_ ? this->correct_entry_.hangul : this->correct_entry_.latin));
                // Center text in the question circle
                const sf::FloatRect text_bounds = this->question_text_.getLocalBounds();
                this->question_text_.setOrigin(text_bounds.left + text_bounds.width / 2.0f,
                                               text_bounds.top + text_bounds.height / 2.0f);
                this->question_text_.setPosition(this->question_circle_.getPosition());

                // Setup answer buttons
                for (std::size_t idx = 0; idx < 4; ++idx) {
                    this->button_shapes_[idx].setFillColor(core::colors::default_button);  // Reset button colors
                    this->answer_buttons_[idx].setString(core::string::to_sfml_string(this->is_hangul_ ? options[idx].latin : options[idx].hangul));

                    // Center text in the answer buttons
                    const sf::FloatRect ans_text_bounds = this->answer_buttons_[idx].getLocalBounds();
                    this->answer_buttons_[idx].setOrigin(ans_text_bounds.left + ans_text_bounds.width / 2.0f,
                                                         ans_text_bounds.top + ans_text_bounds.height / 2.0f);
                    this->answer_buttons_[idx].setPosition(this->button_shapes_[idx].getPosition());
                }
            }
        };

        auto update_percentage_text = [&]() {
            const float percentage = this->total_questions_ > 0 ? (static_cast<float>(this->correct_answers_) / static_cast<float>(this->total_questions_)) * 100.f : 0.f;
            const auto percentage_str = fmt::format("게임 점수: {:.1f}%", percentage);
            this->percentage_text_.setString(core::string::to_sfml_string(percentage_str));
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
                            setup_new_question();
                            break;
                        }
                    }
                }

                if (this->game_state_ == GameState::WaitingForAnswer) {
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
                                this->game_state_ = GameState::ShowResult;
                                ++this->total_questions_;
                                // Highlight the buttons based on the user's selection
                                if (idx == this->correct_index_) {
                                    ++this->correct_answers_;
                                    this->button_shapes_[idx].setFillColor(core::colors::correct_answer);  // Correct answer
                                }
                                else {
                                    this->button_shapes_[idx].setFillColor(core::colors::selected_wrong_answer);            // Chosen wrong answer (orange)
                                    this->button_shapes_[this->correct_index_].setFillColor(core::colors::correct_answer);  // Highlight correct answer
                                }
                                // Set the other buttons to incorrect color
                                for (std::size_t jdx = 0; jdx < 4; ++jdx) {
                                    if (jdx != idx && jdx != this->correct_index_) {
                                        this->button_shapes_[jdx].setFillColor(core::colors::incorrect_answer);  // Incorrect answers
                                    }
                                }
                                update_percentage_text();
                                break;
                            }
                        }
                    }
                    else if (event.type == sf::Event::KeyPressed) {
                        // Handle keyboard input
                        std::size_t selected_index = static_cast<std::size_t>(-1);
                        if (event.key.code == sf::Keyboard::Num1 || event.key.code == sf::Keyboard::Numpad1) {
                            selected_index = 0;
                        }
                        else if (event.key.code == sf::Keyboard::Num2 || event.key.code == sf::Keyboard::Numpad2) {
                            selected_index = 1;
                        }
                        else if (event.key.code == sf::Keyboard::Num3 || event.key.code == sf::Keyboard::Numpad3) {
                            selected_index = 2;
                        }
                        else if (event.key.code == sf::Keyboard::Num4 || event.key.code == sf::Keyboard::Numpad4) {
                            selected_index = 3;
                        }
                        if (selected_index != static_cast<std::size_t>(-1)) {
                            this->game_state_ = GameState::ShowResult;
                            ++this->total_questions_;
                            // Highlight the buttons based on the user's selection
                            if (selected_index == this->correct_index_) {
                                ++this->correct_answers_;
                                this->button_shapes_[selected_index].setFillColor(core::colors::correct_answer);  // Correct answer
                            }
                            else {
                                this->button_shapes_[selected_index].setFillColor(core::colors::selected_wrong_answer);  // Chosen wrong answer (orange)
                                this->button_shapes_[this->correct_index_].setFillColor(core::colors::correct_answer);   // Highlight correct answer
                            }
                            // Set the other buttons to incorrect color
                            for (std::size_t jdx = 0; jdx < 4; ++jdx) {
                                if (jdx != selected_index && jdx != this->correct_index_) {
                                    this->button_shapes_[jdx].setFillColor(core::colors::incorrect_answer);  // Incorrect answers
                                }
                            }
                            update_percentage_text();
                        }
                    }
                }
                else if (this->game_state_ == GameState::ShowResult) {
                    if (event.type == sf::Event::MouseButtonReleased || event.type == sf::Event::KeyPressed) {
                        // Proceed to the next question
                        setup_new_question();
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
            }

            // Handle hover effect when moving the mouse without any events
            if (this->game_state_ == GameState::WaitingForAnswer) {
                for (std::size_t idx = 0; idx < 4; ++idx) {
                    if (this->button_shapes_[idx].getGlobalBounds().contains(static_cast<float>(mouse_pos.x), static_cast<float>(mouse_pos.y))) {
                        this->button_shapes_[idx].setFillColor(core::colors::hover_button);
                    }
                    else {
                        this->button_shapes_[idx].setFillColor(core::colors::default_button);
                    }
                }
            }
            else if (this->game_state_ == GameState::NoEntriesEnabled) {
                // Disable hover effect on answer buttons
                for (auto &button_shape : this->button_shapes_) {
                    button_shape.setFillColor(core::colors::disabled_color);
                }
            }

            this->window_.clear(core::colors::background);  // Dark background color
            this->window_.draw(this->question_circle_);
            this->window_.draw(this->question_text_);
            for (std::size_t idx = 0; idx < 4; ++idx) {
                this->window_.draw(this->button_shapes_[idx]);
                this->window_.draw(this->answer_buttons_[idx]);
            }
            this->window_.draw(this->percentage_text_);
            // Draw toggle buttons
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
    core::vocabulary::Vocabulary vocabulary_;

    enum class GameState {
        WaitingForAnswer,
        ShowResult,
        NoEntriesEnabled
    };
    GameState game_state_;

    core::vocabulary::Entry correct_entry_;
    std::vector<sf::Text> answer_buttons_;
    std::size_t correct_index_;
    bool is_hangul_;

    // Score tracking
    std::size_t total_questions_;
    std::size_t correct_answers_;

    // UI Elements
    sf::CircleShape question_circle_;
    sf::Text question_text_;
    std::vector<sf::CircleShape> button_shapes_;
    sf::Text percentage_text_;

    // Toggle button states
    std::vector<std::string> toggle_labels_;
    std::vector<core::vocabulary::Category> toggle_categories_;
    std::unordered_map<core::vocabulary::Category, bool> toggle_states_;
    std::vector<sf::RectangleShape> toggle_buttons_;
    std::vector<sf::Text> toggle_texts_;
};

}  // namespace

void run()
{
    UI().run();
}

}  // namespace app
