/**
 * @file vocabulary.cpp
 */

#include <algorithm>  // for std::shuffle
#include <cstddef>    // for std::size_t
#include <vector>     // for std::vector

#include "core/rng.hpp"
#include "vocabulary.hpp"

namespace modules::vocabulary {

Vocabulary::Vocabulary()
    : entries{
          // Basic Vowels
          {"ㅏ", "a", Category::BasicVowel},
          {"ㅑ", "ya", Category::BasicVowel},
          {"ㅓ", "eo", Category::BasicVowel},
          {"ㅕ", "yeo", Category::BasicVowel},
          {"ㅗ", "o", Category::BasicVowel},
          {"ㅛ", "yo", Category::BasicVowel},
          {"ㅜ", "u", Category::BasicVowel},
          {"ㅠ", "yu", Category::BasicVowel},
          {"ㅡ", "eu", Category::BasicVowel},
          {"ㅣ", "i", Category::BasicVowel},

          // Basic Consonants
          {"ㄱ", "g", Category::BasicConsonant},
          {"ㄴ", "n", Category::BasicConsonant},
          {"ㄷ", "d", Category::BasicConsonant},
          {"ㄹ", "r/l", Category::BasicConsonant},
          {"ㅁ", "m", Category::BasicConsonant},
          {"ㅂ", "b", Category::BasicConsonant},
          {"ㅅ", "s", Category::BasicConsonant},
          {"ㅇ", "ng", Category::BasicConsonant},
          {"ㅈ", "j", Category::BasicConsonant},
          {"ㅊ", "ch", Category::BasicConsonant},
          {"ㅋ", "k", Category::BasicConsonant},
          {"ㅌ", "t", Category::BasicConsonant},
          {"ㅍ", "p", Category::BasicConsonant},
          {"ㅎ", "h", Category::BasicConsonant},

          // Double Consonants
          {"ㄲ", "kk", Category::DoubleConsonant},
          {"ㄸ", "tt", Category::DoubleConsonant},
          {"ㅃ", "pp", Category::DoubleConsonant},
          {"ㅆ", "ss", Category::DoubleConsonant},
          {"ㅉ", "jj", Category::DoubleConsonant},

          // Compound Vowels
          {"ㅐ", "ae", Category::CompoundVowel},
          {"ㅒ", "yae", Category::CompoundVowel},
          {"ㅔ", "e", Category::CompoundVowel},
          {"ㅖ", "ye", Category::CompoundVowel},
          {"ㅘ", "wa", Category::CompoundVowel},
          {"ㅙ", "wae", Category::CompoundVowel},
          {"ㅚ", "oe", Category::CompoundVowel},
          {"ㅝ", "wo", Category::CompoundVowel},
          {"ㅞ", "we", Category::CompoundVowel},
          {"ㅟ", "wi", Category::CompoundVowel},
          {"ㅢ", "ui", Category::CompoundVowel}},
      category_enabled{{Category::BasicVowel, true}, {Category::BasicConsonant, true}, {Category::DoubleConsonant, true}, {Category::CompoundVowel, true}}
{
}

Entry Vocabulary::get_random_entry()
{
    // Collect enabled entries
    std::vector<Entry> enabled_entries;
    for (const auto &entry : this->entries) {
        if (this->category_enabled.at(entry.category)) {
            enabled_entries.push_back(entry);
        }
    }

    if (enabled_entries.empty()) {
        return {"N/A", "N/A", Category::BasicVowel};
    }

    const auto index = core::rng::RNG::get_random_number<std::size_t>(0, enabled_entries.size() - 1);
    return enabled_entries[index];
}

Entry Vocabulary::get_random_wrong_entry(const Entry &correct_entry)
{
    std::vector<Entry> wrong_entries;
    for (const auto &entry : this->entries) {
        if (this->category_enabled.at(entry.category) && entry.hangul != correct_entry.hangul) {
            wrong_entries.push_back(entry);
        }
    }

    if (wrong_entries.empty()) {
        return {"N/A", "N/A", Category::BasicVowel};
    }

    const auto index = core::rng::RNG::get_random_number<std::size_t>(0, wrong_entries.size() - 1);
    return wrong_entries[index];
}

std::vector<Entry> Vocabulary::get_question_options(const Entry &correct_entry,
                                                    const std::size_t num_options)
{
    std::vector<Entry> options = {correct_entry};

    // Collect possible wrong entries
    std::vector<Entry> wrong_entries;
    for (const auto &entry : this->entries) {
        if (this->category_enabled.at(entry.category) && entry.hangul != correct_entry.hangul) {
            wrong_entries.push_back(entry);
        }
    }

    // Shuffle wrong entries
    std::shuffle(wrong_entries.begin(), wrong_entries.end(), core::rng::RNG::instance());

    // Add unique wrong entries until we have the desired number of options
    for (const auto &wrong_entry : wrong_entries) {
        if (options.size() >= num_options) {
            break;
        }
        options.push_back(wrong_entry);
    }

    // Shuffle the options
    std::shuffle(options.begin(), options.end(), core::rng::RNG::instance());

    return options;
}

void Vocabulary::set_category_enabled(const Category category,
                                      const bool enabled)
{
    this->category_enabled[category] = enabled;
}

}  // namespace modules::vocabulary
