#include "model/CarbonationFactory.hpp"

#include "model/Machine.hpp"
#include "model/Product.hpp"

#include <memory>

namespace gactorio {

namespace {

RecipeMemento exportRecipeState(const Recipe& recipe) {
    RecipeMemento state;
    state.id = recipe.id();
    state.name = recipe.name();
    state.durationSeconds = recipe.durationSeconds();
    state.inputs = recipe.inputs();
    state.outputs = recipe.outputs();
    return state;
}

Recipe makeRecipe(const RecipeMemento& state) {
    Recipe recipe(state.id, state.name, state.durationSeconds);
    for (const auto& input : state.inputs) {
        recipe.addInput(input.first, input.second);
    }
    for (const auto& output : state.outputs) {
        recipe.addOutput(output.first, output.second);
    }
    return recipe;
}

} // namespace

CarbonationFactory::CarbonationFactory() {
    Recipe sodaCan(1, "Soda Can", 4.0);
    sodaCan.addInput(ItemType::Water, 1);
    sodaCan.addInput(ItemType::Syrup, 1);
    sodaCan.addInput(ItemType::CarbonDioxide, 1);
    sodaCan.addInput(ItemType::Can, 1);
    sodaCan.addInput(ItemType::Label, 1);
    sodaCan.addOutput(static_cast<ProductId>(ProductType::SodaCan), 1);
    recipes_.push_back(sodaCan);

    Recipe sparklingWater(2, "Sparkling Water", 3.0);
    sparklingWater.addInput(ItemType::Water, 1);
    sparklingWater.addInput(ItemType::CarbonDioxide, 1);
    sparklingWater.addInput(ItemType::Can, 1);
    sparklingWater.addInput(ItemType::Label, 1);
    sparklingWater.addOutput(static_cast<ProductId>(ProductType::SparklingWater), 1);
    recipes_.push_back(sparklingWater);

    inventory().addItem(ItemType::Water, 100);
    inventory().addItem(ItemType::Syrup, 100);
    inventory().addItem(ItemType::CarbonDioxide, 100);
    inventory().addItem(ItemType::Can, 100);
    inventory().addItem(ItemType::Caffeine, 100);
    inventory().addItem(ItemType::Label, 100);

    ProductionLine line(1, "Beverage Line");
    line.addMachine(std::make_unique<Carbonator>(1, "Carbonator"));
    line.addMachine(std::make_unique<Filler>(2, "Filler"));
    line.addMachine(std::make_unique<Sealer>(3, "Sealer"));
    line.addMachine(std::make_unique<Labeler>(4, "Labeler"));
    addProductionLine(std::move(line));

    (void)enqueueProduct(1, std::make_unique<SparklingWater>());
}

const std::vector<Recipe>& CarbonationFactory::recipes() const {
    return recipes_;
}

std::vector<RecipeMemento> CarbonationFactory::exportRecipeStates() const {
    std::vector<RecipeMemento> states;
    states.reserve(recipes_.size());
    for (const auto& recipe : recipes_) {
        states.push_back(exportRecipeState(recipe));
    }
    return states;
}

void CarbonationFactory::restoreRecipeStates(const std::vector<RecipeMemento>& recipes) {
    recipes_.clear();
    recipes_.reserve(recipes.size());
    for (const auto& recipe : recipes) {
        recipes_.push_back(makeRecipe(recipe));
    }
}

} // namespace gactorio
