/**
 * @file vocabulary.cpp
 */

#include <algorithm>      // for std::shuffle
#include <cstddef>        // for std::size_t
#include <optional>       // for std::optional, std::nullopt
#include <stdexcept>      // for std::runtime_error
#include <unordered_map>  // for std::unordered_map
#include <vector>         // for std::vector

#include <fmt/core.h>

#include "core/math/rng.hpp"
#include "vocabulary.hpp"

namespace modules::vocabulary {

Vocabulary::Vocabulary()
    // Transliteration reference: http://letslearnhangul.com/
    // Note: The number of entries in each category must be greater than 3 (i.e., 4, 5, 6, 7, 8, 9, etc.).
    // If 3 or fewer entries are present in a category, the generate_enabled_question_options will throw an exception and the program will crash.
    // The automated tests count the number of entries in each category to ensure this requirement is met.
    : entries_{
          // Basic vowels
          {"ㅏ", "a", "Looks like an 'a' without the crossbar", Category::BasicVowel},
          {"ㅑ", "ya", "It's 'ㅏ' with an extra line (adds 'y')", Category::BasicVowel},
          {"ㅓ", "eo", "Think of 'eo' as 'uh' sound", Category::BasicVowel},
          {"ㅕ", "yeo", "It's 'ㅓ' with an extra line (adds 'y')", Category::BasicVowel},
          {"ㅗ", "o", "Line 'o'ver the bar", Category::BasicVowel},
          {"ㅛ", "yo", "It's 'ㅗ' with an extra line (adds 'y')", Category::BasicVowel},
          {"ㅜ", "u", "Line 'u'nder the bar", Category::BasicVowel},
          {"ㅠ", "yu", "It's 'ㅜ' with an extra line (adds 'y')", Category::BasicVowel},
          {"ㅡ", "eu", "A horizontal line, sounds like 'oo' in 'good'", Category::BasicVowel},
          {"ㅣ", "i", "Looks like the letter 'i'", Category::BasicVowel},
          {"ㅐ", "ae", "'ㅏ' plus an extra line", Category::BasicVowel},
          {"ㅔ", "e", "'ㅓ' plus an extra line", Category::BasicVowel},

          // Basic consonants
          {"ㄱ", "g/k", "Looks like a 'gun'", Category::BasicConsonant},
          {"ㄴ", "n", "Nike swoosh or 'n' rotated", Category::BasicConsonant},
          {"ㄷ", "d/t", "Door frame shape", Category::BasicConsonant},
          {"ㄹ", "r/l", "Resembles 'r' and 'l' combined", Category::BasicConsonant},
          {"ㅁ", "m", "Looks like a mouth", Category::BasicConsonant},
          {"ㅂ", "b/p", "Bucket shape", Category::BasicConsonant},
          {"ㅅ", "s", "Looks like a mountain", Category::BasicConsonant},
          {"ㅇ", "-/ng", "Circle like 'zero' sound", Category::BasicConsonant},
          {"ㅈ", "j", "Looks like 'ㅅ' with a line", Category::BasicConsonant},
          {"ㅊ", "ch", "It's 'ㅈ' with an extra line on top", Category::BasicConsonant},
          {"ㅋ", "k", "Looks like a 'key'", Category::BasicConsonant},
          {"ㅌ", "t", "Looks like a 't' with a hat", Category::BasicConsonant},
          {"ㅍ", "p", "Looks like a 'pi' symbol", Category::BasicConsonant},
          {"ㅎ", "h", "Man with a hat on", Category::BasicConsonant},

          // Double consonants
          {"ㄲ", "kk", "Double 'ㄱ'", Category::DoubleConsonant},
          {"ㄸ", "tt", "Double 'ㄷ'", Category::DoubleConsonant},
          {"ㅃ", "pp", "Double 'ㅂ'", Category::DoubleConsonant},
          {"ㅆ", "ss", "Double 'ㅅ'", Category::DoubleConsonant},
          {"ㅉ", "jj", "Double 'ㅈ'", Category::DoubleConsonant},

          // Compound vowels
          {"ㅒ", "yae", "Combination of 'ㅑ' and 'ㅣ'", Category::CompoundVowel},
          {"ㅖ", "ye", "Combination of 'ㅕ' and 'ㅣ'", Category::CompoundVowel},
          {"ㅘ", "wa", "'ㅗ' plus 'ㅏ'", Category::CompoundVowel},
          {"ㅙ", "wae", "'ㅗ' plus 'ㅐ'", Category::CompoundVowel},
          {"ㅚ", "oe", "'ㅗ' plus 'ㅣ'", Category::CompoundVowel},
          {"ㅝ", "wo", "'ㅜ' plus 'ㅓ'", Category::CompoundVowel},
          {"ㅞ", "we", "'ㅜ' plus 'ㅔ'", Category::CompoundVowel},
          {"ㅟ", "wi", "'ㅜ' plus 'ㅣ'", Category::CompoundVowel},
          {"ㅢ", "ui", "'ㅡ' plus 'ㅣ'", Category::CompoundVowel}}
{
    // Reduce memory usage
    this->entries_.shrink_to_fit();
}

std::optional<Entry> Vocabulary::get_random_enabled_entry(const std::unordered_map<Category, bool> &category_enabled)
{
    // Collect enabled entries
    std::vector<Entry> enabled_entries;
    for (const auto &entry : this->entries_) {
        if (category_enabled.at(entry.category)) {
            enabled_entries.emplace_back(entry);
        }
    }

    if (enabled_entries.empty()) {
        return std::nullopt;
    }

    const std::size_t index = core::math::rng::get_random_number<std::size_t>(0, enabled_entries.size() - 1);
    return enabled_entries[index];
}

std::vector<Entry> Vocabulary::generate_enabled_question_options(const Entry &correct_entry,
                                                                 const std::unordered_map<Category, bool> &category_enabled,
                                                                 const std::size_t num_options)
{
    std::vector<Entry> options;
    options.reserve(num_options);
    options.emplace_back(correct_entry);

    // Collect possible wrong entries
    std::vector<Entry> wrong_entries;
    wrong_entries.reserve(this->entries_.size());
    for (const auto &entry : this->entries_) {
        if (category_enabled.at(entry.category) && entry.hangul != correct_entry.hangul) {
            wrong_entries.emplace_back(entry);
        }
    }

    // Shuffle wrong entries
    std::shuffle(wrong_entries.begin(), wrong_entries.end(), core::math::rng::instance());

    // Add unique wrong entries until we have the desired number of options
    for (const auto &wrong_entry : wrong_entries) {
        if (options.size() >= num_options) {
            break;
        }
        options.emplace_back(wrong_entry);
    }

    // Throw if the number of options is less than the desired number
    // This will NEVER happen unless someone messes with the data, it's only here for nice error messages
    if (const std::size_t len = options.size(); len < num_options) {
        throw std::runtime_error(fmt::format("Generated '{}' question options, but '{}' were requested; each category in vocabulary needs at least {} entries", len, num_options, num_options));
    }

    // Shuffle the options
    std::shuffle(options.begin(), options.end(), core::math::rng::instance());

    // Return shrunk vector (RVO)
    options.shrink_to_fit();
    return options;
}

const std::vector<Entry> &Vocabulary::get_entries() const
{
    return this->entries_;
}

}  // namespace modules::vocabulary
