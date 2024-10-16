/**
 * @file test_all.cpp
 */

#include <algorithm>      // for std::sort
#include <cstddef>        // for std::size_t
#include <cstdlib>        // for EXIT_FAILURE, EXIT_SUCCESS
#include <exception>      // for std::exception
#include <filesystem>     // for std::filesystem
#include <fstream>        // for std::ofstream
#include <functional>     // for std::function
#include <stdexcept>      // for std::runtime_error
#include <string>         // for std::string
#include <unordered_map>  // for std::unordered_map
#include <vector>         // for std::vector

#include <fmt/core.h>

// #include "app.hpp"
// #include "core/args.hpp"
// #include "core/string.hpp"
// #include "modules/analyze.hpp"
#if defined(_WIN32)
#include "core/io.hpp"
#endif

// #include "examples.hpp"
// #include "helpers.hpp"

#define TEST_EXECUTABLE_NAME "tests"

namespace test_dummy {
[[nodiscard]] int dummy();
}

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
        {"test_dummy::dummy", test_dummy::dummy},
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

int test_dummy::dummy()
{
    fmt::print("Hello world!\n");
    return EXIT_SUCCESS;
}
