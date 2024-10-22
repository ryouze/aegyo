/**
 * @file vocabulary.hpp
 *
 * @brief Define the Korean vocabulary.
 */

#pragma once

#include <cstddef>        // for std::size_t
#include <string>         // for std::string
#include <unordered_map>  // for std::unordered_map
#include <vector>         // for std::vector

namespace modules::vocabulary {

/**
 * @brief Enum that represents the category of a Korean character.
 */
enum class Category {
    BasicVowel,
    BasicConsonant,
    DoubleConsonant,
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
     */
    explicit Vocabulary();

    /**
     * @brief Get the random entry object from the vocabulary.
     *
     * @return Random Entry object where the category is enabled or an entry with "N/A" if no categories are enabled.
     */
    [[nodiscard]] Entry get_random_entry();

    /**
     * @brief Get the random wrong entry object from the vocabulary.
     *
     * @param correct_entry Correct Entry object that should not be included in the result.
     *
     * @return Random Entry object where the category is enabled and the hangul is not the same as the correct entry provided or an entry with "N/A" if no suitable entries are found.
     */
    [[nodiscard]] Entry get_random_wrong_entry(const Entry &correct_entry);

    /**
     * @brief Get a set of unique options for a question.
     *
     * @param correct_entry Correct Entry object that should be included in the options.
     * @param num_options Total number of options to generate (default: 4).
     *
     * @return Vector of Entry objects representing the question options.
     *
     * @throws std::runtime_error if the size of the vector is less than "num_options because there are not enough unique entries.
     */
    [[nodiscard]] std::vector<Entry> get_question_options(const Entry &correct_entry,
                                                          const std::size_t num_options = 4);

    /**
     * @brief Enable or disable a category in the vocabulary.
     *
     * @param category Category to enable or disable.
     * @param enabled Whether to enable or disable the category.
     */
    void set_category_enabled(const Category category,
                              const bool enabled);

  private:
    /**
     * @brief Vector of all vocabulary entries.
     */
    std::vector<Entry> entries;

    /**
     * @brief Map indicating whether each category is enabled.
     */
    std::unordered_map<Category, bool> category_enabled;
};

}  // namespace modules::vocabulary
