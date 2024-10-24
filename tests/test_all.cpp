/**
 * @file test_all.cpp
 */

#include <cstdlib>        // for EXIT_FAILURE, EXIT_SUCCESS
#include <exception>      // for std::exception
#include <functional>     // for std::function
#include <random>         // for std::mt19937, std::shuffle
#include <string>         // for std::string
#include <unordered_map>  // for std::unordered_map
#include <vector>         // for std::vector

#include <SFML/Graphics.hpp>
#include <fmt/core.h>

#include "core/assets.hpp"
#include "core/rng.hpp"
#include "core/string.hpp"
#include "modules/vocabulary.hpp"
#if defined(_WIN32)
#include "core/io.hpp"
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
[[nodiscard]] int to_sfml_string();
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
#if defined(_WIN32)
    // Setup UTF-8 input/output on Windows (does nothing on other platforms)
    if (const auto e = core::io::setup_utf8_console(); e.has_value()) {
        fmt::print(stderr, "Warning: {}\n", *e);
    }
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
        {"test_string::to_sfml_string", test_string::to_sfml_string},
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
        const sf::Font &font = core::assets::load_font();
        // Get font properties
        const std::string family = font.getInfo().family;
        const std::string expected_family = "NanumGothic";
        if (family != expected_family) {
            fmt::print(stderr, "core::assets::load_font() failed: the actual font family '{}' is not equal to expected '{}'\n", family, expected_family);
            return EXIT_FAILURE;
        }
        const bool smooth = font.isSmooth();
        constexpr bool expected_smooth = true;
        if (smooth != expected_smooth) {
            fmt::print(stderr, "core::assets::load_font() failed: the actual font smooth property '{}' is not equal to expected '{}'\n", smooth, expected_smooth);
            return EXIT_FAILURE;
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
        std::mt19937 &rng = core::rng::RNG::instance();
        std::shuffle(cars.begin(), cars.end(), rng);
        fmt::print("core::rng::RNG::instance() passed.\n");
        return EXIT_SUCCESS;
    }
    catch (const std::exception &e) {
        fmt::print(stderr, "core::rng::RNG::instance() failed: {}\n", e.what());
        return EXIT_FAILURE;
    }
}

int test_rng::get_random_number()
{
    try {
        // Generate random numbers
        constexpr int min = 0;
        constexpr int max = 10;
        const int random_number = core::rng::RNG::get_random_number<int>(min, max);
        if (random_number < min || random_number > max) {
            fmt::print(stderr, "core::rng::RNG::get_random_number() failed: the actual random number '{}' is not in the range [{}, {}]\n", random_number, min, max);
            return EXIT_FAILURE;
        }
        fmt::print("core::rng::RNG::get_random_number() passed.\n");
        return EXIT_SUCCESS;
    }
    catch (const std::exception &e) {
        fmt::print(stderr, "core::rng::RNG::get_random_number() failed: {}\n", e.what());
        return EXIT_FAILURE;
    }
}

int test_rng::get_random_bool()
{
    try {
        // Generate random boolean values
        constexpr double probability = 0.5;
        const bool random_bool = core::rng::RNG::get_random_bool(probability);
        if (random_bool != true && random_bool != false) {
            fmt::print(stderr, "core::rng::RNG::get_random_bool() failed: the actual random boolean value '{}' is not true or false\n", random_bool);
            return EXIT_FAILURE;
        }
        fmt::print("core::rng::RNG::get_random_bool() passed.\n");
        return EXIT_SUCCESS;
    }
    catch (const std::exception &e) {
        fmt::print(stderr, "core::rng::RNG::get_random_bool() failed: {}\n", e.what());
        return EXIT_FAILURE;
    }
}

int test_string::to_sfml_string()
{
    try {
        // Convert a UTF-8 string to an SFML string
        const std::string utf8_str = "Dzień dobry";
        const sf::String sfml_str = core::string::to_sfml_string(utf8_str);
        if (sfml_str.isEmpty()) {
            fmt::print(stderr, "core::string::to_sfml_string() failed: the SFML string is empty\n");
            return EXIT_FAILURE;
        }
        fmt::print("core::string::to_sfml_string() passed.\n");
        return EXIT_SUCCESS;
    }
    catch (const std::exception &e) {
        fmt::print(stderr, "core::string::to_sfml_string() failed: {}\n", e.what());
        return EXIT_FAILURE;
    }
}

int test_vocabulary::entry()
{
    try {
        // Create a vocabulary entry
        const modules::vocabulary::Entry entry = {"ㅏ", "a", modules::vocabulary::Category::BasicVowel};
        if (entry.hangul != "ㅏ") {
            fmt::print(stderr, "modules::vocabulary::Entry failed: the actual Korean character '{}' is not equal to expected 'ㅏ'\n", entry.hangul);
            return EXIT_FAILURE;
        }
        if (entry.latin != "a") {
            fmt::print(stderr, "modules::vocabulary::Entry failed: the actual Latin transliteration '{}' is not equal to expected 'a'\n", entry.latin);
            return EXIT_FAILURE;
        }
        if (entry.category != modules::vocabulary::Category::BasicVowel) {
            fmt::print(stderr, "modules::vocabulary::Entry failed: the actual category '{}' is not equal to expected 'Category::BasicVowel'\n", static_cast<int>(entry.category));
            return EXIT_FAILURE;
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
        modules::vocabulary::Vocabulary vocabulary;

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
                fmt::print(stderr, "modules::vocabulary::Vocabulary failed: Category '{}' has less than {} entries (found {}).\n",
                           static_cast<int>(category), min_category_entries, count);
                return EXIT_FAILURE;
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
