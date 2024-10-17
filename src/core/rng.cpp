/**
 * @file rng.cpp
 */

#include <random>  // for std::mt19937, std::random_device, std::uniform_int_distribution, std::bernoulli_distribution

#include "rng.hpp"

namespace core::rng {

std::mt19937 &RNG::instance()
{
    static std::mt19937 rng{std::random_device{}()};
    return rng;
}

int RNG::get_random_int(const int min,
                        const int max)
{
    std::uniform_int_distribution<int> dist(min, max);
    return dist(RNG::instance());
}

bool RNG::get_random_bool(const double probability)
{
    std::bernoulli_distribution dist(probability);
    return dist(RNG::instance());
}

}  // namespace core::rng
