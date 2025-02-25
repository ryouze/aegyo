/**
 * @file test_all.cpp
 */

#include <cstddef>        // for std::size_t
#include <cstdlib>        // for EXIT_FAILURE, EXIT_SUCCESS
#include <exception>      // for std::exception
#include <functional>     // for std::function
#include <random>         // for std::mt19937, std::shuffle
#include <stdexcept>      // for std::runtime_error
#include <string>         // for std::string
#include <unordered_map>  // for std::unordered_map
#include <vector>         // for std::vector

#include <SFML/Graphics.hpp>
#include <fmt/core.h>

#include "core/graphics/font.hpp"
#include "core/math/rng.hpp"
#include "modules/vocabulary.hpp"
#include "ui/components/base.hpp"
#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN  // Exclude rarely-used stuff from Windows headers
#include <windows.h>         // for SetConsoleCP, SetConsoleOutputCP, CP_UTF8
#endif

namespace test_assets {
[[nodiscard]] int load_font();
}

namespace test_rng {
[[nodiscard]] int instance();
[[nodiscard]] int get_random_number();
[[nodiscard]] int get_random_bool();
}  // namespace test_rng

namespace test_string {
[[nodiscard]] int text();
}

namespace test_vocabulary {
[[nodiscard]] int entry();
[[nodiscard]] int category_count();
}  // namespace test_vocabulary

/**
 * @brief Entry-point of the test application.
 *
 * @param argc Number of command-line arguments (e.g., "2").
 * @param argv Array of command-line arguments (e.g., {"./bin", "-h"}).
 *
 * @return EXIT_SUCCESS if the test application ran successfully, EXIT_FAILURE otherwise.
 */
int main(int argc,
         char **argv)
{
#if defined(_WIN32)  // Setup UTF-8 input/output
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
#endif

    // Define the formatted help message
    const std::string help_message = fmt::format(
        "Usage: {} <test>\n"
        "\n"
        "Run automatic tests.\n"
        "\n"
        "Positional arguments:\n"
        "  test  name of the test to run ('all' to run all tests)\n",
        argv[0]);

    // If no arguments, print help message and exit
    if (argc == 1) {
        fmt::print("{}\n", help_message);
        return EXIT_FAILURE;
    }

    // Otherwise, define argument to function mapping
    const std::unordered_map<std::string, std::function<int()>> tests = {
        {"test_assets::load_font", test_assets::load_font},
        {"test_rng::instance", test_rng::instance},
        {"test_rng::get_random_number", test_rng::get_random_number},
        {"test_rng::get_random_bool", test_rng::get_random_bool},
        {"test_string::text", test_string::text},
        {"test_vocabulary::entry", test_vocabulary::entry},
        {"test_vocabulary::category_count", test_vocabulary::category_count},
    };

    // Get the test name from the command-line arguments
    const std::string arg = argv[1];

    // If the test name is found, run the corresponding test
    if (const auto it = tests.find(arg); it != tests.cend()) {
        try {
            return it->second();
        }
        catch (const std::exception &e) {
            fmt::print(stderr, "Test '{}' threw an exception: {}\n", arg, e.what());
            return EXIT_FAILURE;
        }
    }
    else if (arg == "all") {
        // Run all tests sequentially and print the results
        bool all_passed = true;
        for (const auto &[name, test_func] : tests) {
            fmt::print("Running test: {}\n", name);
            try {
                const int result = test_func();
                if (result != EXIT_SUCCESS) {
                    all_passed = false;
                    fmt::print(stderr, "Test '{}' failed.\n", name);
                }
                else {
                    fmt::print("Test '{}' passed.\n", name);
                }
            }
            catch (const std::exception &e) {
                all_passed = false;
                fmt::print(stderr, "Test '{}' threw an exception: {}\n", name, e.what());
            }
        }
        return all_passed ? EXIT_SUCCESS : EXIT_FAILURE;
    }
    else {
        fmt::print(stderr, "Error: Invalid test name: '{}'\n\n{}\n", arg, help_message);
        return EXIT_FAILURE;
    }
}

int test_assets::load_font()
{
    try {
        // Load embedded NanumGothic font
        std::unique_ptr<sf::Font> font = core::graphics::font::load();  // Ownership is transferred here
        const ui::components::base::Text text(*font);
        // Get font properties
        const std::string family = font->getInfo().family;
        const std::string expected_family = "NanumGothic";
        if (family != expected_family) {
            throw std::runtime_error(fmt::format("The actual font family '{}' is not equal to expected '{}'", family, expected_family));
        }
        const bool smooth = font->isSmooth();
        constexpr bool expected_smooth = true;
        if (smooth != expected_smooth) {
            throw std::runtime_error(fmt::format("The actual font smooth property '{}' is not equal to expected '{}'", smooth, expected_smooth));
        }
        fmt::print("core::assets::load_font() passed.\n");
        return EXIT_SUCCESS;
    }
    catch (const std::exception &e) {
        fmt::print(stderr, "core::assets::load_font() failed: {}\n", e.what());
        return EXIT_FAILURE;
    }
}

int test_rng::instance()
{
    try {
        // Define a vector of cars
        std::vector<std::string> cars = {"Nissan Skyline GT-R R32", "Toyota Supra Mk4", "Mazda RX-7 FD", "Honda NSX"};

        // Shuffle the cars using the random number generator
        std::mt19937 &rng = core::math::rng::instance();
        std::shuffle(cars.begin(), cars.end(), rng);
        fmt::print("core::rng::instance() passed.\n");
        return EXIT_SUCCESS;
    }
    catch (const std::exception &e) {
        fmt::print(stderr, "core::rng::instance() failed: {}\n", e.what());
        return EXIT_FAILURE;
    }
}

int test_rng::get_random_number()
{
    try {
        // Generate random numbers
        constexpr std::size_t min = 0;
        constexpr std::size_t max = 10;
        const std::size_t random_number = core::math::rng::get_random_number<std::size_t>(min, max);
        if (random_number < min || random_number > max) {
            throw std::runtime_error(fmt::format("The actual random number '{}' is not in the range [{}, {}]", random_number, min, max));
        }
        fmt::print("core::rng::get_random_number() passed.\n");
        return EXIT_SUCCESS;
    }
    catch (const std::exception &e) {
        fmt::print(stderr, "core::rng::get_random_number() failed: {}\n", e.what());
        return EXIT_FAILURE;
    }
}

int test_rng::get_random_bool()
{
    try {
        // Generate random boolean values
        constexpr double probability = 0.5;
        const bool random_bool = core::math::rng::get_random_bool(probability);
        static_cast<void>(random_bool);  // Ignore [[nodiscard]] attribute
        fmt::print("core::rng::get_random_bool() passed.\n");
        return EXIT_SUCCESS;
    }
    catch (const std::exception &e) {
        fmt::print(stderr, "core::rng::get_random_bool() failed: {}\n", e.what());
        return EXIT_FAILURE;
    }
}

int test_string::text()
{
    try {
        // Load embedded NanumGothic font
        std::unique_ptr<sf::Font> font = core::graphics::font::load();  // Ownership is transferred here
        ui::components::base::Text text(*font);

        // Convert a UTF-8 string to an SFML string
        const std::string utf8_str = "Dzień dobry";
        text.setString(utf8_str);
        if (text.getString().isEmpty()) {
            throw std::runtime_error("The SFML string is empty");
        }
        fmt::print("core::string::text() passed.\n");
        return EXIT_SUCCESS;
    }
    catch (const std::exception &e) {
        fmt::print(stderr, "core::string::text() failed: {}\n", e.what());
        return EXIT_FAILURE;
    }
}

int test_vocabulary::entry()
{
    try {
        // Create a vocabulary entry
        const modules::vocabulary::Entry entry = {"ㅏ", "a", "Looks like an 'a' without the crossbar", modules::vocabulary::Category::BasicVowel};
        if (entry.hangul != "ㅏ") {
            throw std::runtime_error(fmt::format("The actual Korean character '{}' is not equal to expected 'ㅏ'", entry.hangul));
        }
        if (entry.latin != "a") {
            throw std::runtime_error(fmt::format("The actual Latin transliteration '{}' is not equal to expected 'a'", entry.latin));
        }
        if (entry.memo != "Looks like an 'a' without the crossbar") {
            throw std::runtime_error(fmt::format("The actual memo '{}' is not equal to expected 'Looks like an 'a' without the crossbar'", entry.memo));
        }
        if (entry.category != modules::vocabulary::Category::BasicVowel) {
            throw std::runtime_error(fmt::format("The actual category '{}' is not equal to expected 'Category::BasicVowel'", static_cast<int>(entry.category)));
        }
        fmt::print("modules::vocabulary::Entry passed.\n");
        return EXIT_SUCCESS;
    }
    catch (const std::exception &e) {
        fmt::print(stderr, "modules::vocabulary::Entry failed: {}\n", e.what());
        return EXIT_FAILURE;
    }
}

int test_vocabulary::category_count()
{
    try {
        // Create a vocabulary object
        const modules::vocabulary::Vocabulary vocabulary;

        // Map to store the count of entries for each category
        std::unordered_map<modules::vocabulary::Category, std::size_t> category_counts;

        // Count the number of entries for each category
        for (const auto &entry : vocabulary.get_entries()) {
            ++category_counts[entry.category];
        }

        // Ensure each category has more than 3 entries
        const std::size_t min_category_entries = 4;
        for (const auto &[category, count] : category_counts) {
            if (count < min_category_entries) {
                throw std::runtime_error(fmt::format("Category '{}' has less than {} entries (found {})", static_cast<int>(category), min_category_entries, count));
            }
            fmt::print("modules::vocabulary::Vocabulary: Category '{}' has {} entries.\n", static_cast<int>(category), count);
        }

        fmt::print("modules::vocabulary::Vocabulary passed.\n");
        return EXIT_SUCCESS;
    }
    catch (const std::exception &e) {
        fmt::print(stderr, "modules::vocabulary::Vocabulary failed: {}\n", e.what());
        return EXIT_FAILURE;
    }
}
