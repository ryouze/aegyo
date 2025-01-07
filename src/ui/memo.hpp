/**
 * @file memo.hpp
 *
 * @brief Hint memo.
 */

#pragma once

#include <string>   // for std::string

#include <SFML/Graphics.hpp>

#include "core/assets/font.hpp"
#include "core/settings/colors.hpp"
#include "core/settings/screen.hpp"

namespace ui::memo {

class Memo {
  public:
    explicit Memo()
    {
        // Appearance
        this->text_.setCharacterSize(16);
        this->text_.setFillColor(core::settings::colors::text);

        // Position
        this->text_.setPosition({core::settings::screen::CENTER.x,
                                 core::settings::screen::CENTER.y - 30.f});
    }

    void hide()
    {
        this->text_.setString("");
    }

    void set(const std::string &str)
    {
        this->text_.setString(str);
        this->text_.resetOrigin();
    }
    /**
     * @brief Draw the text to the window.
     *
     * @param window Window to draw to.
     */
    void draw(sf::RenderWindow &window) const
    {
        window.draw(this->text_);
    }

  private:
    core::assets::font::Text text_;
};

}  // namespace ui::memo
