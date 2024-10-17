/**
 * @file rng.hpp
 *
 * @brief Random number generation.
 */

#pragma once

#include <random>  // for std::mt19937

namespace core::rng {

/**
 * @brief Singleton class that provides a static random number generator.
 *
 * @note This class is marked as `final` to prevent inheritance.
 */
class RNG final {
  public:
    /**
     * @brief Get the static random number generator instance.
     *
     * @return Reference to the static instance of "std::mt19937" random number generator.
     */
    [[nodiscard]] static std::mt19937 &instance();

    /**
     * @brief Get a random number in the range [min, max].
     *
     * @tparam T Numeric type (must be an integral or unsigned type).
     *@param min Minimum value (e.g., "0").
     * @param max Maximum value (e.g., "10").
     *
     * @return Random number in the specified range (e.g., "5").
     *
     * @note The range is inclusive for both the minimum and maximum values.
     */
    template <typename T>
    [[nodiscard]] static T get_random_number(const T min,
                                             const T max);

    /**
     * @brief Get a random boolean value with a given probability of being true.
     *
     * @param probability Probability of returning true between 0.0 and 1.0 (default: 0.5).
     *
     * @return Random boolean value (e.g., "true").
     */
    [[nodiscard]] static bool get_random_bool(const double probability = 0.5);
};

}  // namespace core::rng
