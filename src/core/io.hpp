/**
 * @file io.hpp
 *
 * @brief Input/output functions.
 */

#pragma once

#if defined(_WIN32)

#include <optional>  // for std::optional
#include <string>    // for std::string

namespace core::io {

// /**
//  * @brief Attach to the console if it exists. Do nothing on other platforms.
//  *
//  * This ensures that when the application is run from the command line, it can print to the console.
//  *
//  * @return Error message if the attachment fails, "std::nullopt" otherwise.
//  */
// [[nodiscard]] std::optional<std::string> attach_to_console();

/**
 * @brief Setup UTF-8 input/output on Windows. Do nothing on other platforms.
 *
 * @return Error message if the setup fails, "std::nullopt" otherwise.
 */
[[nodiscard]] std::optional<std::string> setup_utf8_console();

}  // namespace core::io

#endif
