/**
 * @file main.cpp
 */

#include <cstdlib>    // for EXIT_FAILURE, EXIT_SUCCESS
#include <exception>  // for std::exception

#include <fmt/core.h>

#include "app.hpp"
#if defined(_WIN32)
#include "core/io.hpp"
#endif

/**
 * @brief Entry-point of the application.
 *
 * @return EXIT_SUCCESS if the application ran successfully, EXIT_FAILURE otherwise.
 */
int main()
{
    try {
#if defined(_WIN32)
        // Boilerplate to make Windows behave more like *nix

        // DEBUG: Display output to the console if started from the command line
        // if (const auto e = core::io::attach_to_console(); e.has_value()) {
        // fmt::print(stderr, "Warning: {}\n", *e);
        // }

        // Setup UTF-8 input/output and locale
        if (const auto e = core::io::setup_utf8_console(); e.has_value()) {
            fmt::print(stderr, "Warning: {}\n", *e);
        }
#endif

        // Run the app
        app::run();
    }
    catch (const std::exception &e) {
        fmt::print(stderr, "{}\n", e.what());
        return EXIT_FAILURE;
    }
    catch (...) {
        fmt::print(stderr, "Error: Unknown\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
