#include "../../include/model/Recipe.hpp"

#include <sstream>

namespace factory {

Recipe::Recipe()
    : recipeName(""),
      targetCaffeineMg(0),
      targetSugarGram(0),
      targetVolumeMl(0) {
}

Recipe::Recipe(
    const std::string& recipeName,
    int targetCaffeineMg,
    int targetSugarGram,
    int targetVolumeMl
)
    : recipeName(recipeName),
      targetCaffeineMg(targetCaffeineMg),
      targetSugarGram(targetSugarGram),
      targetVolumeMl(targetVolumeMl) {
}

void Recipe::addIngredient(const Ingredient& ingredient) {
    ingredients.push_back(ingredient);
}

std::string Recipe::getRecipeName() const {
    return recipeName;
}

std::vector<Ingredient> Recipe::getIngredients() const {
    return ingredients;
}

int Recipe::getTargetCaffeineMg() const {
    return targetCaffeineMg;
}

int Recipe::getTargetSugarGram() const {
    return targetSugarGram;
}

int Recipe::getTargetVolumeMl() const {
    return targetVolumeMl;
}

std::string Recipe::getInfo() const {
    std::ostringstream oss;

    oss << "===== Recipe Info =====\n";
    oss << "Recipe Name: " << recipeName << "\n";
    oss << "Target Caffeine: " << targetCaffeineMg << " mg\n";
    oss << "Target Sugar: " << targetSugarGram << " g\n";
    oss << "Target Volume: " << targetVolumeMl << " ml\n";

    oss << "Required Ingredients:\n";

    if (ingredients.empty()) {
        oss << "- No ingredients registered.\n";
    } else {
        for (const Ingredient& ingredient : ingredients) {
            oss << "- "
                << ingredient.getName()
                << ": "
                << ingredient.getAmount()
                << " "
                << ingredient.getUnit()
                << "\n";
        }
    }

    return oss.str();
}

}