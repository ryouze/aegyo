/**
 * @file app.cpp
 */

#include <array>          // for std::array
#include <cstddef>        // for std::size_t
#include <limits>         // for std::numeric_limits
#include <optional>       // for std::optional
#include <string>         // for std::string
#include <unordered_map>  // for std::unordered_map

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <fmt/core.h>

#include "app.hpp"
#include "core/assets.hpp"
#include "core/rng.hpp"
#include "core/string.hpp"
#include "modules/vocabulary.hpp"
#include "version.hpp"
#if defined(_WIN32)
#include "core/io.hpp"
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
        : window_(
              sf::VideoMode({800, 600}),
              fmt::format("aegyo ({})", PROJECT_VERSION),
              sf::State::Windowed,
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
          question_circle_(),
          question_text_(this->font_, "", 48),
          memo_text_(this->font_, "", 16),
          percentage_text_(this->font_, "", 18),  // Smaller font size, top-left corner
          button_shapes_(),
          answer_buttons_({sf::Text(this->font_, "", 28),
                           sf::Text(this->font_, "", 28),
                           sf::Text(this->font_, "", 28),
                           sf::Text(this->font_, "", 28)}),
          toggle_buttons_({sf::RectangleShape(),
                           sf::RectangleShape(),
                           sf::RectangleShape(),
                           sf::RectangleShape()}),
          toggle_texts_({sf::Text(this->font_),
                         sf::Text(this->font_),
                         sf::Text(this->font_),
                         sf::Text(this->font_)})
    {
        // Enable V-Sync to limit the frame rate to the refresh rate of the monitor
        this->window_.setVerticalSyncEnabled(true);

        // Disable key repeat, as we only want one key press to register
        this->window_.setKeyRepeatEnabled(false);

        // Log anti-aliasing level
        // fmt::print("Anti-aliasing level: {}\n", this->window_.getSettings().antialiasingLevel);

        // Set window titlebar icon (Windows-only)
        // macOS doesn't have titlebar icons, GNU/Linux is DE-dependent
#if defined(_WIN32)
        if (const auto e = core::io::setup_titlebar_icon(this->window_); e.has_value()) {
            fmt::print(stderr, "Warning: {}\n", *e);
        }
#endif

        // Initialize UI elements
        // Initialize question circle
        this->question_circle_.setRadius(80.f);
        this->question_circle_.setPointCount(100);
        this->question_circle_.setOrigin({80.f, 80.f});
        this->question_circle_.setPosition({400.f, 150.f});
        this->question_circle_.setFillColor(this->colors_question_circle);

        // Initialize question text
        this->question_text_.setFillColor(this->colors_text);
        this->question_text_.setPosition(this->question_circle_.getPosition());

        // Initialize memo text
        this->memo_text_.setFillColor(this->colors_text);
        this->memo_text_.setPosition({400.f, 270.f});  // Position below the question circle

        // Initialize answer buttons
        constexpr float button_radius = 60.f;
        for (std::size_t idx = 0; idx < 4; ++idx) {
            this->button_shapes_[idx].setRadius(button_radius);
            this->button_shapes_[idx].setPointCount(100);
            this->button_shapes_[idx].setOrigin({button_radius, button_radius});
            this->button_shapes_[idx].setFillColor(this->colors_default_button);
        }
        this->button_shapes_[0].setPosition({250.f, 350.f});
        this->button_shapes_[1].setPosition({550.f, 350.f});
        this->button_shapes_[2].setPosition({250.f, 500.f});
        this->button_shapes_[3].setPosition({550.f, 500.f});

        // Set positions of answer button texts
        for (std::size_t idx = 0; idx < 4; ++idx) {
            this->answer_buttons_[idx].setFillColor(this->colors_text);
            this->answer_buttons_[idx].setPosition(this->button_shapes_[idx].getPosition());
        }

        // Initialize percentage text
        this->percentage_text_.setFillColor(this->colors_text);
        this->percentage_text_.setPosition({10.f, 10.f});  // Position below the question circle

        // Initialize toggle buttons
        constexpr float total_toggle_width = 4.f * 60.f;  // 4 buttons * 60 width
        const float start_x =
            static_cast<float>(this->window_.getSize().x) - total_toggle_width - 10.f;  // 10.f padding from the right
        for (std::size_t idx = 0; idx < 4; ++idx) {
            this->toggle_buttons_[idx].setSize({50.f, 35.f});
            // Set fill color based on initial state
            if (this->toggle_states_.at(this->toggle_categories_[idx])) {
                this->toggle_buttons_[idx].setFillColor(this->colors_enabled_toggle);  // Enabled state color
            }
            else {
                this->toggle_buttons_[idx].setFillColor(this->colors_disabled_toggle);  // Disabled state color
            }
            this->toggle_buttons_[idx].setOutlineColor(this->colors_text);
            this->toggle_buttons_[idx].setOutlineThickness(1.f);
            this->toggle_buttons_[idx].setPosition({start_x + static_cast<float>(idx) * 60.f, 10.f});  // Positioned in the top-right corner

            this->toggle_texts_[idx].setString(this->toggle_labels_[idx]);
            this->toggle_texts_[idx].setCharacterSize(14);
            this->toggle_texts_[idx].setFillColor(this->colors_text);
            // Center text in the button
            const auto tbounds = this->toggle_texts_[idx].getLocalBounds();
            const auto ox = tbounds.position.x + tbounds.size.x / 2.f;
            const auto oy = tbounds.position.y + tbounds.size.y / 2.f;
            this->toggle_texts_[idx].setOrigin({ox, oy});
            this->toggle_texts_[idx].setPosition(
                this->toggle_buttons_[idx].getPosition() + sf::Vector2f(25.f, 17.5f));
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
            const float percentage = total_questions > 0 ? (static_cast<float>(correct_answers) / static_cast<float>(total_questions)) * 100.f : 0.f;
            const auto percentage_str = fmt::format("게임 점수: {:.1f}%", percentage);
            this->percentage_text_.setString(core::string::to_sfml_string(percentage_str));
        };

        update_percentage_text();

        const auto setup_new_question = [&]() {
            const auto optional_entry = this->vocabulary_.get_random_enabled_entry(this->toggle_states_);

            // No entries enabled; display 'X' in the question circle
            if (!optional_entry.has_value()) {
                this->question_text_.setString("X");
                this->question_text_.setCharacterSize(72);  // Increase font size for the 'X'
                // Center text in the question circle
                const sf::FloatRect text_bounds = this->question_text_.getLocalBounds();
                this->question_text_.setOrigin({text_bounds.position.x + text_bounds.size.x * 0.5f,

                                                text_bounds.position.y + text_bounds.size.y * 0.5f});
                game_state = GameState::NoEntriesEnabled;
                // Disable answer buttons visually
                for (auto &button_shape : this->button_shapes_) {
                    button_shape.setFillColor(this->colors_disabled_toggle);
                }
                for (auto &answer_button : this->answer_buttons_) {
                    answer_button.setString("");
                }
                // Clear memo text
                this->memo_text_.setString("");
            }
            else {
                // Entries are enabled; setup new question
                correct_entry = optional_entry.value();

                is_hangul = core::rng::RNG::get_random_bool();

                const auto options = this->vocabulary_.generate_enabled_question_options(correct_entry, this->toggle_states_);

                for (std::size_t idx = 0; idx < 4; ++idx) {
                    if (options[idx].hangul == correct_entry.hangul) {
                        correct_index = idx;
                        break;
                    }
                }

                this->question_text_.setCharacterSize(48);  // Reset to default size
                this->question_text_.setString(core::string::to_sfml_string(is_hangul ? correct_entry.hangul : correct_entry.latin));
                // Center text in the question circle
                const auto question_bounds = this->question_text_.getLocalBounds();
                this->question_text_.setOrigin({question_bounds.position.x + question_bounds.size.x * 0.5f,
                                                question_bounds.position.y + question_bounds.size.y * 0.5f});

                // Clear memo text
                this->memo_text_.setString("");

                // Setup answer buttons
                for (std::size_t idx = 0; idx < 4; ++idx) {
                    this->button_shapes_[idx].setFillColor(this->colors_default_button);  // Reset button colors
                    this->answer_buttons_[idx].setString(core::string::to_sfml_string(is_hangul ? options[idx].latin : options[idx].hangul));

                    // Center text in the answer buttons
                    const auto answer_bounds = this->answer_buttons_[idx].getLocalBounds();
                    const auto ox = answer_bounds.position.x + answer_bounds.size.x / 2.f;
                    const auto oy = answer_bounds.position.y + answer_bounds.size.y / 2.f;

                    // Set position to center the text within the button
                    this->answer_buttons_[idx].setOrigin({ox, oy});
                    this->answer_buttons_[idx].setPosition(this->button_shapes_[idx].getPosition());
                }

                game_state = GameState::WaitingForAnswer;
            }
        };

        setup_new_question();

        // Main loop
        while (this->window_.isOpen()) {
            // Variables for event handling
            while (const std::optional event = this->window_.pollEvent()) {
                if (event->is<sf::Event::Closed>()) {
                    this->window_.close();
                }

                // Handle toggle button clicks
                else if (const auto mb = event->getIf<sf::Event::MouseButtonReleased>()) {
                    if (mb->button == sf::Mouse::Button::Left) {
                        const sf::Vector2f mouse_position(
                            static_cast<float>(sf::Mouse::getPosition(this->window_).x),
                            static_cast<float>(sf::Mouse::getPosition(this->window_).y));
                        for (std::size_t idx = 0; idx < this->toggle_buttons_.size(); ++idx) {
                            if (this->toggle_buttons_[idx].getGlobalBounds().contains(mouse_position)) {
                                // Toggle the category
                                const bool current_state = this->toggle_states_[this->toggle_categories_[idx]];
                                this->toggle_states_[this->toggle_categories_[idx]] = !current_state;
                                // Update button appearance
                                if (this->toggle_states_[this->toggle_categories_[idx]]) {
                                    this->toggle_buttons_[idx].setFillColor(this->colors_enabled_toggle);  // Enabled state color
                                }
                                else {
                                    this->toggle_buttons_[idx].setFillColor(this->colors_disabled_toggle);  // Disabled state color
                                }
                                // Reset the game
                                total_questions = 0;
                                correct_answers = 0;
                                update_percentage_text();
                                setup_new_question();
                                break;
                            }
                        }
                        // Handle hover effect for toggle buttons
                        if (game_state == GameState::WaitingForAnswer) {
                            for (std::size_t i = 0; i < 4; ++i) {
                                if (this->button_shapes_[i].getGlobalBounds().contains(mouse_position)) {
                                    ++total_questions;
                                    if (i == correct_index) {
                                        ++correct_answers;
                                        this->button_shapes_[i].setFillColor(this->colors_correct_answer);
                                    }
                                    else {
                                        this->button_shapes_[i].setFillColor(this->colors_selected_wrong_answer);
                                        this->button_shapes_[correct_index].setFillColor(this->colors_correct_answer);
                                    }
                                    for (std::size_t j = 0; j < 4; ++j) {
                                        if (j != i && j != correct_index) {
                                            this->button_shapes_[j].setFillColor(this->colors_incorrect_answer);
                                        }
                                    }
                                    update_percentage_text();
                                    this->memo_text_.setString(
                                        core::string::to_sfml_string(correct_entry.memo));
                                    const auto mbounds = this->memo_text_.getLocalBounds();
                                    this->memo_text_.setOrigin({mbounds.position.x + mbounds.size.x * 0.5f,
                                                                mbounds.position.y + mbounds.size.y * 0.5f});
                                    game_state = GameState::ShowResult;
                                    break;
                                }
                            }
                        }
                        else if (game_state == GameState::ShowResult) {
                            this->memo_text_.setString("");
                            setup_new_question();
                        }
                    }
                }
                else if (const auto mm = event->getIf<sf::Event::MouseMoved>()) {
                    const sf::Vector2f mousePos(
                        static_cast<float>(mm->position.x),
                        static_cast<float>(mm->position.y));
                    for (std::size_t idx = 0; idx < this->toggle_buttons_.size(); ++idx) {
                        if (this->toggle_buttons_[idx].getGlobalBounds().contains(mousePos)) {
                            this->toggle_buttons_[idx].setOutlineThickness(2.f);
                        }
                        else {
                            this->toggle_buttons_[idx].setOutlineThickness(1.f);
                        }
                    }

                    // Game logic
                    if (game_state == GameState::WaitingForAnswer) {
                        // Handle hover effect for answer buttons
                        for (std::size_t idx = 0; idx < 4; ++idx) {
                            if (this->button_shapes_[idx].getGlobalBounds().contains(mousePos)) {
                                this->button_shapes_[idx].setFillColor(this->colors_hover_button);
                            }
                            else {
                                this->button_shapes_[idx].setFillColor(this->colors_default_button);
                            }
                        }
                    }
                }
                else if (const auto kp = event->getIf<sf::Event::KeyPressed>()) {
                    // Handle keyboard input
                    if (game_state == GameState::WaitingForAnswer) {
                        std::size_t selected_index = std::numeric_limits<std::size_t>::max();
                        switch (kp->scancode) {
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
                        if (selected_index < 4) {
                            ++total_questions;
                            if (selected_index == correct_index) {
                                ++correct_answers;
                                this->button_shapes_[selected_index].setFillColor(this->colors_correct_answer);
                            }
                            else {
                                this->button_shapes_[selected_index].setFillColor(this->colors_selected_wrong_answer);
                                this->button_shapes_[correct_index].setFillColor(this->colors_correct_answer);
                            }
                            for (std::size_t j = 0; j < 4; ++j) {
                                if (j != selected_index && j != correct_index) {
                                    this->button_shapes_[j].setFillColor(this->colors_incorrect_answer);
                                }
                            }
                            update_percentage_text();
                            // Display memo text
                            this->memo_text_.setString(core::string::to_sfml_string(correct_entry.memo));
                            // Center memo text
                            const auto mbounds = this->memo_text_.getLocalBounds();
                            this->memo_text_.setOrigin({mbounds.position.x + mbounds.size.x * 0.5f,
                                                        mbounds.position.y + mbounds.size.y * 0.5f});
                            game_state = GameState::ShowResult;
                        }
                    }
                    else if (game_state == GameState::ShowResult) {
                        // Proceed to the next question
                        // Hide memo text
                        this->memo_text_.setString("");
                        setup_new_question();
                    }
                }
            }

            // Render
            this->window_.clear(this->colors_background);
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
    const std::array<std::string, 4> toggle_labels_;
    const std::array<modules::vocabulary::Category, 4> toggle_categories_;
    std::unordered_map<modules::vocabulary::Category, bool> toggle_states_;

    // UI Elements
    sf::CircleShape question_circle_;
    sf::Text question_text_;
    sf::Text memo_text_;
    sf::Text percentage_text_;

    std::array<sf::CircleShape, 4> button_shapes_;
    std::array<sf::Text, 4> answer_buttons_;

    std::array<sf::RectangleShape, 4> toggle_buttons_;
    std::array<sf::Text, 4> toggle_texts_;

    // Colors
    // Background color
    inline static const sf::Color colors_background = sf::Color(30, 30, 30);

    // Text color
    inline static const sf::Color colors_text = sf::Color(240, 240, 240);

    // Button colors
    inline static const sf::Color colors_enabled_toggle = sf::Color(100, 200, 100);   // Green for enabled
    inline static const sf::Color colors_disabled_toggle = sf::Color(200, 100, 100);  // Red for disabled

    // Answer button colors
    inline static const sf::Color colors_default_button = sf::Color(80, 80, 80);
    inline static const sf::Color colors_hover_button = sf::Color(100, 100, 100);
    inline static const sf::Color colors_correct_answer = sf::Color(30, 130, 30);
    inline static const sf::Color colors_incorrect_answer = sf::Color(130, 30, 30);
    inline static const sf::Color colors_selected_wrong_answer = sf::Color(200, 120, 0);

    // Question circle color
    inline static const sf::Color colors_question_circle = sf::Color(50, 50, 50);
};

}  // namespace

void run()
{
    UI().run();
}

}  // namespace app
