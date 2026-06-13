#include "model/Product.hpp"
#include "model/ProductCatalog.hpp"
#include "model/ProductionTask.hpp"
#include "model/config/DefinitionRegistry.hpp"
#include "model/config/FactoryConfigLoader.hpp"

#include <cassert>
#include <string>

int main() {
    gactorio::ProcessStep legacy(gactorio::MachineRole::Mixing, 3.0);
    assert(legacy.stepKind() == "mixing");
    assert(legacy.requiredRole() == gactorio::MachineRole::Mixing);
    assert(legacy.hasLegacyRequiredRole());
    assert(legacy.legacyRequiredRole().has_value());
    assert(*legacy.legacyRequiredRole() == gactorio::MachineRole::Mixing);
    assert(legacy.durationSeconds() == 3.0);

    gactorio::ProcessStep known("packaging", 4.0);
    assert(known.stepKind() == "packaging");
    assert(known.requiredRole() == gactorio::MachineRole::Packaging);
    assert(known.hasLegacyRequiredRole());
    assert(known.legacyRequiredRole().has_value());
    assert(*known.legacyRequiredRole() == gactorio::MachineRole::Packaging);

    gactorio::ProcessStep unknown("flavoring", 5.0);
    assert(unknown.stepKind() == "flavoring");
    assert(unknown.requiredRole() == gactorio::MachineRole::Unknown);
    assert(!unknown.hasLegacyRequiredRole());
    assert(!unknown.legacyRequiredRole().has_value());
    assert(unknown.baseDurationSeconds() == 5.0);

    const std::string json = R"json(
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
          "id": "flavor",
          "stepKind": "flavoring",
          "duration": 5.0,
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
  ]
}
)json";

    const auto config = gactorio::config_model::FactoryConfigLoader::loadFromString(json);
    const gactorio::config_model::DefinitionRegistry registry(config);
    const auto definition = gactorio::makeProductDefinitionFromRegistry(registry, "experimental");
    assert(definition.has_value());
    assert(definition->route.size() == 1);
    assert(definition->route[0].stepKind() == "flavoring");
    assert(definition->route[0].requiredRole() == gactorio::MachineRole::Unknown);
    assert(!definition->route[0].legacyRequiredRole().has_value());

    auto product = gactorio::createProductFromRegistry(registry, "experimental");
    assert(product != nullptr);
    gactorio::ProductionTask task(product);
    assert(task.currentStep() != nullptr);
    assert(task.currentStep()->stepKind() == "flavoring");
    assert(task.currentStepKind() == "flavoring");

    task.advanceStep();
    assert(task.isCompleted());
    assert(task.currentStepKind().empty());

    return 0;
}
