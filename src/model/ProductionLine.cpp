#include "../../include/model/ProductionLine.hpp"

namespace factory {

ProductionLine::ProductionLine() {
}

void ProductionLine::addFactory(std::shared_ptr<Factory> factory) {
    if (factory == nullptr) {
        return;
    }

    factories.push_back(factory);
}

Product ProductionLine::run(const Recipe& recipe, Inventory& inventory) {
    Product product(
        recipe.getRecipeName(),
        recipe.getTargetCaffeineMg(),
        recipe.getTargetSugarGram(),
        recipe.getTargetVolumeMl(),
        false
    );

    for (const Ingredient& ingredient : recipe.getIngredients()) {
        const std::string& name = ingredient.getName();
        double requiredAmount = ingredient.getAmount();

        if (!inventory.hasIngredient(name, requiredAmount)) {
            return Product();
        }

        inventory.useIngredient(name, requiredAmount);
        product.addIngredient(name);
    }

    for (const std::shared_ptr<Factory>& factory : factories) {
        if (factory != nullptr) {
            product = factory->process(product, recipe, inventory);
        }
    }

    return product;
}

int ProductionLine::getFactoryCount() const {
    return static_cast<int>(factories.size());
}

}