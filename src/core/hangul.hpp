/**
 * @file hangul.hpp
 *
 * @brief Define the Korean vocabulary.
 */

#pragma once

#include <cstddef>        // for std::size_t
#include <optional>       // for std::optional
#include <string>         // for std::string
#include <unordered_map>  // for std::unordered_map
#include <vector>         // for std::vector

namespace core::hangul {

/**
 * @brief Enum that represents the category of a Korean character.
 */
enum class Category {
    /**
     * @brief Basic vowel (e.g., "ㅏ").
     */
    BasicVowel,

    /**
     * @brief Basic consonant (e.g., "ㄱ").
     */
    BasicConsonant,

    /**
     * @brief Double consonant (e.g., "ㄲ").
     */
    DoubleConsonant,

    /**
     * @brief Compound vowel (e.g., "ㅐ").
     */
    CompoundVowel
};

/**
 * @brief Struct that represents a single entry in the Korean vocabulary.
 *
 * @note This struct is marked as `final` to prevent inheritance.
 */
struct Entry final {
    /**
     * @brief Korean character (e.g., "ㅏ").
     */
    std::string hangul;

    /**
     * @brief Latin transliteration (e.g., "a").
     */
    std::string latin;

    /**
     * @brief Short memo to help remember the character.
     */
    std::string memo;

    /**
     * @brief Category of the Korean character (e.g., "Category::BasicVowel").
     */
    Category category;
};

/**
 * @brief Class that manages the Korean vocabulary.
 *
 * On construction, the class initializes the vocabulary with a set of Korean characters and their Latin equivalents.
 *
 * @note This class is marked as `final` to prevent inheritance.
 */
class Vocabulary final {
  public:
    /**
     * @brief Construct a new Vocabulary object.
     *
     * On construction, initialize the vocabulary with a hardcoded set of Korean characters and their Latin equivalents.
     */
    explicit Vocabulary();

    /**
     * @brief Get a random entry from the vocabulary where the category is enabled.
     *
     * @param category_enabled Map of categories and their enabled state.
     *
     * @return Entry object where the category is enabled, or std::nullopt if no categories are enabled.
     */
    [[nodiscard]] std::optional<Entry> get_random_enabled_entry(const std::unordered_map<Category, bool> &category_enabled);

    /**
     * @brief Get a set of unique options for a question.
     *
     * @param correct_entry Correct Entry object that should be included in the options.
     * @param category_enabled Map of categories and their enabled state.
     * @param num_options Total number of options to generate (default: 4).
     *
     * @return Vector of Entry objects representing the question options.
     *
     * @throws std::runtime_error if the size of the vector is less than "num_options" because there are not enough unique entries.
     *
     * @note The exception will NEVER be thrown unless someone modifies the vocabulary data in the constructor, which will be caught by the CI anyway. Therefore, it is safe to assume that the exception will NEVER be thrown in practice and it's only there for nice error messages in case someone messes with the data.
     */
    [[nodiscard]] std::vector<Entry> generate_enabled_question_options(const Entry &correct_entry,
                                                                       const std::unordered_map<Category, bool> &category_enabled,
                                                                       const std::size_t num_options = 4);

    /**
     * @brief Get a vector of all vocabulary entries.
     *
     * @return Const reference to a vector of Entry objects.
     *
     * @note This is used by the CI to ensure that there are enough entries for each category.
     */
    [[nodiscard]] const std::vector<Entry> &get_entries() const;

  private:
    /**
     * @brief Vector of all vocabulary entries.
     */
    std::vector<Entry> entries_;
};

}  // namespace core::hangul
