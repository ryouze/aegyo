/**
 * @file rng.cpp
 */

#include <cstddef>      // for std::size_t
#include <random>       // for std::mt19937, std::random_device, std::uniform_int_distribution, std::bernoulli_distribution
#include <type_traits>  // for std::is_integral_v

#include "rng.hpp"

namespace core::rng {

std::mt19937 &RNG::instance()
{
    static std::mt19937 rng{std::random_device{}()};
    return rng;
}

template <typename T>
T RNG::get_random_number(const T min,
                         const T max)
{
    static_assert(std::is_integral_v<T>, "get_random_number requires an integral or unsigned type.");
    std::uniform_int_distribution<T> dist(min, max);
    return dist(RNG::instance());
}

bool RNG::get_random_bool(const double probability)
{
    std::bernoulli_distribution dist(probability);
    return dist(RNG::instance());
}

// Explicit template instantiations
template int RNG::get_random_number<int>(int, int);
template std::size_t RNG::get_random_number<std::size_t>(std::size_t, std::size_t);

}  // namespace core::rng
