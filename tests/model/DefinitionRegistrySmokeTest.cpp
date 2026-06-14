#include "model/config/DefinitionRegistry.hpp"

#include <cassert>

namespace {

gactorio::config_model::FactoryConfig makeValidConfig() {
    using namespace gactorio::config_model;

    FactoryConfig config;
    config.schemaVersion = "1.0.0";
    config.items = {
        ItemDefinition{"ingredient", "Ingredient", "raw", true, 5},
        ItemDefinition{"water", "Water", "raw", true, 5},
    };
    config.products = {
        ProductDefinition{
            "voltz_classic",
            "Voltz Classic",
            "Standard energy drink",
            "standard",
            "#F4C430",
            10,
            "voltz_classic_recipe"},
    };
    config.stations = {
        StationDefinition{
            "mixing_station",
            "mixing",
            "Mixer",
            "Mixing Station",
            {},
            1.0,
            150.0,
            0.02},
        StationDefinition{
            "quality_station",
            "quality",
            "Quality",
            "Quality Station",
            {"quality"},
            1.0,
            100.0,
            0.03},
    };
    config.recipes = {
        RecipeDefinition{
            "voltz_classic_recipe",
            "voltz_classic",
            "Voltz Classic Recipe",
            {
                RecipeStepDefinition{
                    "mixing",
                    "mixing",
                    13.0,
                    {ItemStackDefinition{"ingredient", 2}, ItemStackDefinition{"water", 1}},
                    {}},
                RecipeStepDefinition{
                    "quality",
                    "quality",
                    9.0,
                    {},
                    {RecipeOutputDefinition{std::nullopt, ProductId{"voltz_classic"}, 1}}},
            }},
    };
    config.lines = {
        ProductionLineDefinition{
            "line_a",
            "Line A",
            {"mixing_station", "quality_station"},
            {"voltz_classic_recipe"},
            std::nullopt},
    };
    config.initialInventory = {
        InventorySeed{"ingredient", 5},
        InventorySeed{"water", 5},
    };
    config.startupTasks = {
        StartupTaskDefinition{"line_a", "voltz_classic_recipe", 1},
    };
    config.scenarios = {
        ScenarioDefinition{
            "bottleneck",
            "Bottleneck",
            "Slow mixing",
            {},
            {"mixing"},
            0.5,
            std::nullopt,
            std::nullopt},
    };
    return config;
}

template <typename Fn>
bool throwsRegistryError(Fn&& fn) {
    try {
        fn();
    } catch (const gactorio::config_model::DefinitionRegistryError&) {
        return true;
    }
    return false;
}

} // namespace

int main() {
    using namespace gactorio::config_model;

    const auto validConfig = makeValidConfig();
    const DefinitionRegistry registry(validConfig);
    assert(registry.findItem("ingredient") != nullptr);
    assert(registry.findProduct("voltz_classic") != nullptr);
    assert(registry.findStation("mixing_station") != nullptr);
    assert(registry.findRecipe("voltz_classic_recipe") != nullptr);
    assert(registry.findLine("line_a") != nullptr);
    assert(registry.findScenario("bottleneck") != nullptr);
    assert(registry.requireItem("ingredient").displayName == "Ingredient");
    assert(throwsRegistryError([&registry] { (void)registry.requireItem("missing"); }));
    assert(registry.stationAcceptsStep(registry.requireStation("mixing_station"), "mixing"));
    assert(!registry.stationAcceptsStep(registry.requireStation("mixing_station"), "quality"));
    assert(registry.lineCanProcessRecipe("line_a", "voltz_classic_recipe"));

    auto duplicateItem = makeValidConfig();
    duplicateItem.items.push_back(ItemDefinition{"ingredient", "Ingredient 2", "raw", true, 5});
    assert(throwsRegistryError([&duplicateItem] { DefinitionRegistry registry(duplicateItem); }));

    auto unknownInput = makeValidConfig();
    unknownInput.recipes[0].steps[0].inputs[0].itemId = "syrup_x";
    assert(throwsRegistryError([&unknownInput] { DefinitionRegistry registry(unknownInput); }));

    auto missingStationForStep = makeValidConfig();
    missingStationForStep.lines[0].stationIds = {"mixing_station"};
    assert(throwsRegistryError([&missingStationForStep] { DefinitionRegistry registry(missingStationForStep); }));

    auto disallowedStartupRecipe = makeValidConfig();
    disallowedStartupRecipe.products.push_back(ProductDefinition{
        "hyper_bolt",
        "Hyper Bolt",
        "Premium energy drink",
        "premium",
        "#E63946",
        20,
        "hyper_bolt_recipe"});
    disallowedStartupRecipe.recipes.push_back(RecipeDefinition{
        "hyper_bolt_recipe",
        "hyper_bolt",
        "Hyper Bolt Recipe",
        {
            RecipeStepDefinition{
                "mixing",
                "mixing",
                18.0,
                {ItemStackDefinition{"ingredient", 3}},
                {RecipeOutputDefinition{std::nullopt, ProductId{"hyper_bolt"}, 1}}},
        }});
    disallowedStartupRecipe.startupTasks[0].recipeId = "hyper_bolt_recipe";
    assert(throwsRegistryError([&disallowedStartupRecipe] { DefinitionRegistry registry(disallowedStartupRecipe); }));

    auto emptyAcceptedKindsFallback = makeValidConfig();
    emptyAcceptedKindsFallback.stations[0].acceptedStepKinds.clear();
    const DefinitionRegistry fallbackRegistry(emptyAcceptedKindsFallback);
    assert(fallbackRegistry.stationAcceptsStep(fallbackRegistry.requireStation("mixing_station"), "mixing"));

    return 0;
}
