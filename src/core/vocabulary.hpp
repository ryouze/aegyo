/**
 * @file vocabulary.hpp
 *
 * @brief Define the Korean vocabulary.
 */

#pragma once

#include <cstddef>        // for std::size_t
#include <random>         // for std::mt19937
#include <string>         // for std::string
#include <unordered_map>  // for std::unordered_map
#include <vector>         // for std::vector

namespace core::vocabulary {

enum class Category {
    BasicVowel,
    BasicConsonant,
    DoubleConsonant,
    CompoundVowel
};

struct Entry final {
    std::string hangul;
    std::string latin;
    Category category;
};

class Vocabulary final {
  public:
    Vocabulary();

    // Get a random enabled entry
    Entry get_random_entry();

    // Get a random wrong entry that is not the correct entry
    Entry get_random_wrong_entry(const Entry &correct_entry);

    // Get a set of unique options for a question
    std::vector<Entry> get_question_options(const Entry &correct_entry, std::size_t num_options = 4);

    // Enable or disable a category
    void set_category_enabled(Category category, bool enabled);

  private:
    std::vector<Entry> entries;
    std::unordered_map<Category, bool> category_enabled;
    std::mt19937 rng;  // Random number generator
};

}  // namespace core::vocabulary
