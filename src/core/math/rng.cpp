/**
 * @file rng.cpp
 */

#include <cstddef>      // for std::size_t
#include <random>       // for std::mt19937, std::random_device, std::uniform_int_distribution, std::bernoulli_distribution
#include <type_traits>  // for std::is_integral_v

#include "rng.hpp"

namespace core::math::rng {

std::mt19937 &instance()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    return gen;
}

bool get_random_bool(const double probability)
{
    std::bernoulli_distribution dist(probability);
    return dist(instance());
}

template <typename T>
T get_random_number(const T min,
                    const T max)
{
    static_assert(std::is_integral_v<T>, "get_random_number requires an integral or unsigned type.");
    std::uniform_int_distribution<T> dist(min, max);
    return dist(instance());
}

// Explicit template instantiation
// template int get_random_number<int>(const int min, const int max);
template std::size_t get_random_number<std::size_t>(const std::size_t min, const std::size_t max);

}  // namespace core::math::rng
