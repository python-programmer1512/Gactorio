#include "model/ConfigurableFactory.hpp"
#include "model/ConfiguredStation.hpp"
#include "model/FactoryBuilder.hpp"
#include "model/config/FactoryRuntimeContext.hpp"

#include <cassert>
#include <filesystem>
#include <stdexcept>
#include <string>

int main() {
    const auto context = gactorio::config_model::FactoryRuntimeContext::loadFromFile(
        std::filesystem::path("..") / "data" / "factory_config.runtime.json");

    auto factory = gactorio::FactoryBuilder::createFactory(context);
    assert(factory != nullptr);

    const auto* configurable = dynamic_cast<const gactorio::ConfigurableFactory*>(factory.get());
    assert(configurable != nullptr);
    assert(configurable->recipes().size() == context.config().recipes.size());

    assert(factory->inventory().getItemQuantity("ingredient") == 5);
    assert(factory->inventory().getItemQuantity("water") == 5);
    assert(factory->inventory().getItemQuantity("empty_bottle") == 5);
    assert(factory->inventory().getItemQuantity("label") == 5);
    assert(factory->inventory().getItemQuantity("package") == 5);

    assert(factory->productionLines().size() == context.config().lines.size());
    const auto& line = factory->productionLines().front();
    assert(line.id() == 1);
    assert(line.name() == "Beverage Line A");
    assert(line.machines().size() == 4);
    assert(line.queueLength() == 1);

    const auto* firstStation =
        dynamic_cast<const gactorio::ConfiguredStation*>(line.machines().front().get());
    assert(firstStation != nullptr);
    assert(firstStation->stationDefinitionId() == "mixing_station");
    assert(firstStation->stationKind() == "mixing");
    assert(firstStation->acceptsStep("mixing"));
    assert(!firstStation->acceptsStep("packaging"));

    const auto currentTask = line.currentTask();
    assert(currentTask != nullptr);
    assert(currentTask->getProductId() == "voltz_classic");
    assert(currentTask->recipeId() == "voltz_classic_recipe");

    factory->update(0.1);
    assert(!factory->machines().empty());

    auto fromFile = gactorio::FactoryBuilder::createFactoryFromConfigFile(
        std::filesystem::path("..") / "data" / "factory_config.runtime.json");
    assert(fromFile != nullptr);
    assert(fromFile->productionLines().size() == 1);

    const std::string unsupportedStationKindJson = R"json(
{
  "schemaVersion": "1.0.0",
  "items": [
    { "id": "ingredient", "displayName": "Ingredient" }
  ],
  "products": [
    {
      "id": "experimental",
      "displayName": "Experimental",
      "defaultRecipeId": "experimental_recipe"
    }
  ],
  "stations": [
    {
      "id": "flavor_station",
      "kind": "flavoring",
      "displayName": "Flavor Station",
      "typeName": "Flavor Station",
      "acceptedStepKinds": ["flavoring"]
    }
  ],
  "recipes": [
    {
      "id": "experimental_recipe",
      "productId": "experimental",
      "displayName": "Experimental Recipe",
      "steps": [
        {
          "id": "flavoring",
          "stepKind": "flavoring",
          "duration": 1.0,
          "inputs": [
            { "itemId": "ingredient", "quantity": 1 }
          ],
          "outputs": [
            { "productId": "experimental", "quantity": 1 }
          ]
        }
      ]
    }
  ],
  "lines": [
    {
      "id": "line_a",
      "displayName": "Line A",
      "stationIds": ["flavor_station"],
      "recipeIds": ["experimental_recipe"]
    }
  ],
  "initialInventory": [
    { "itemId": "ingredient", "quantity": 5 }
  ],
  "startupTasks": [
    { "lineId": "line_a", "recipeId": "experimental_recipe", "quantity": 1 }
  ]
}
)json";

    bool sawUnsupportedKind = false;
    try {
        (void)gactorio::FactoryBuilder::createFactoryFromConfigString(unsupportedStationKindJson);
    } catch (const std::invalid_argument&) {
        sawUnsupportedKind = true;
    }
    assert(sawUnsupportedKind);

    return 0;
}
