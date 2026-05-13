#include <iostream>
#include <memory>

#include "Ingredient.hpp"
#include "Recipe.hpp"
#include "Inventory.hpp"
#include "ProductionLine.hpp"
#include "MixingFactory.hpp"
#include "CarbonationFactory.hpp"
#include "PackagingFactory.hpp"

int main() {
    Inventory inventory;
    inventory.addIngredient(Ingredient("Water", 10000, "ml"));
    inventory.addIngredient(Ingredient("Caffeine", 1000, "mg"));
    inventory.addIngredient(Ingredient("Sugar", 1000, "g"));
    inventory.addIngredient(Ingredient("Taurine", 500, "mg"));

    Recipe recipe("Basic Energy Drink", 80, 25, 250);
    recipe.addIngredient(Ingredient("Water", 250, "ml"));
    recipe.addIngredient(Ingredient("Caffeine", 80, "mg"));
    recipe.addIngredient(Ingredient("Sugar", 25, "g"));
    recipe.addIngredient(Ingredient("Taurine", 50, "mg"));

    ProductionLine line;
    line.addFactory(std::make_shared<MixingFactory>());
    line.addFactory(std::make_shared<CarbonationFactory>());
    line.addFactory(std::make_shared<PackagingFactory>());

    Product result = line.run(recipe, inventory);

    std::cout << result.getInfo() << std::endl;

    return 0;
}
