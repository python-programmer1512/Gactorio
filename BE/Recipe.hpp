#pragma once

#include <string>
#include <vector>
#include "Ingredient.hpp"

class Recipe {
private:
    std::string recipeName;
    std::vector<Ingredient> ingredients;
    int targetCaffeineMg;
    int targetSugarGram;
    int targetVolumeMl;

public:
    Recipe();
    Recipe(const std::string& recipeName,
           int targetCaffeineMg,
           int targetSugarGram,
           int targetVolumeMl);

    void addIngredient(const Ingredient& ingredient);

    std::string getRecipeName() const;
    std::vector<Ingredient> getIngredients() const;
    int getTargetCaffeineMg() const;
    int getTargetSugarGram() const;
    int getTargetVolumeMl() const;

    std::string getInfo() const;
};
