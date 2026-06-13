#include "model/ProductCatalog.hpp"
#include "model/ProductionTask.hpp"
#include "model/config/DefinitionRegistry.hpp"
#include "model/config/FactoryConfigLoader.hpp"

#include <cassert>
#include <string>

namespace {

int requirementQuantity(
    const gactorio::ProductDefinition& definition,
    const std::string& itemId) {
    for (const auto& requirement : definition.requirements) {
        if (requirement.itemId() == itemId) {
            return requirement.quantity();
        }
    }
    return 0;
}

} // namespace

int main() {
    gactorio::StepOutput itemOutput;
    itemOutput.itemId = "flavored_base";
    itemOutput.quantity = 1;

    gactorio::ProcessStep directStep(
        "flavoring_step",
        "flavoring",
        gactorio::MachineRole::Unknown,
        6.0,
        {gactorio::ItemRequirement("flavor_syrup", 1)},
        {itemOutput});

    assert(directStep.id() == "flavoring_step");
    assert(directStep.stepKind() == "flavoring");
    assert(directStep.durationSeconds() == 6.0);
    assert(directStep.inputs().size() == 1);
    assert(directStep.inputs()[0].itemId() == "flavor_syrup");
    assert(directStep.inputs()[0].quantity() == 1);
    assert(directStep.outputs().size() == 1);
    assert(directStep.outputs()[0].isItem());
    assert(!directStep.outputs()[0].isProduct());
    assert(directStep.outputs()[0].itemId.value() == "flavored_base");
    assert(directStep.outputs()[0].quantity == 1);

    const std::string json = R"json(
{
  "schemaVersion": "1.0.0",
  "items": [
    { "id": "ingredient", "displayName": "Ingredient" },
    { "id": "flavor_syrup", "displayName": "Flavor Syrup" },
    { "id": "flavored_base", "displayName": "Flavored Base" }
  ],
  "products": [
    {
      "id": "flavored_voltz",
      "displayName": "Flavored Voltz",
      "defaultRecipeId": "flavored_voltz_recipe"
    }
  ],
  "stations": [
    { "id": "mixing_station", "kind": "mixing", "displayName": "Mixer" },
    {
      "id": "flavor_station",
      "kind": "flavoring",
      "displayName": "Flavor Station",
      "acceptedStepKinds": ["flavoring"]
    },
    { "id": "packaging_station", "kind": "packaging", "displayName": "Packager" }
  ],
  "recipes": [
    {
      "id": "flavored_voltz_recipe",
      "productId": "flavored_voltz",
      "displayName": "Flavored Voltz Recipe",
      "steps": [
        {
          "id": "mixing_step",
          "stepKind": "mixing",
          "duration": 3.0,
          "inputs": [
            { "itemId": "ingredient", "quantity": 2 }
          ]
        },
        {
          "id": "flavoring_step",
          "stepKind": "flavoring",
          "duration": 6.0,
          "inputs": [
            { "itemId": "flavor_syrup", "quantity": 1 }
          ],
          "outputs": [
            { "itemId": "flavored_base", "quantity": 1 }
          ]
        },
        {
          "id": "packaging_step",
          "stepKind": "packaging",
          "duration": 2.0,
          "outputs": [
            { "productId": "flavored_voltz", "quantity": 1 }
          ]
        }
      ]
    }
  ],
  "lines": [
    {
      "id": "line_a",
      "displayName": "Line A",
      "stationIds": ["mixing_station", "flavor_station", "packaging_station"],
      "recipeIds": ["flavored_voltz_recipe"]
    }
  ]
}
)json";

    const auto config = gactorio::config_model::FactoryConfigLoader::loadFromString(json);
    const gactorio::config_model::DefinitionRegistry registry(config);
    const auto definition =
        gactorio::makeProductDefinitionFromRegistry(registry, "flavored_voltz");
    assert(definition.has_value());
    assert(definition->route.size() == 3);
    assert(requirementQuantity(*definition, "ingredient") == 2);
    assert(requirementQuantity(*definition, "flavor_syrup") == 1);
    assert(requirementQuantity(*definition, "flavored_base") == 0);

    const auto& flavorStep = definition->route[1];
    assert(flavorStep.id() == "flavoring_step");
    assert(flavorStep.stepKind() == "flavoring");
    assert(flavorStep.requiredRole() == gactorio::MachineRole::Unknown);
    assert(flavorStep.inputs().size() == 1);
    assert(flavorStep.inputs()[0].itemId() == "flavor_syrup");
    assert(flavorStep.outputs().size() == 1);
    assert(flavorStep.outputs()[0].isItem());
    assert(flavorStep.outputs()[0].itemId.value() == "flavored_base");

    const auto& packagingStep = definition->route[2];
    assert(packagingStep.id() == "packaging_step");
    assert(packagingStep.outputs().size() == 1);
    assert(packagingStep.outputs()[0].isProduct());
    assert(packagingStep.outputs()[0].productId.value() == "flavored_voltz");

    auto product = gactorio::createProductFromRegistry(registry, "flavored_voltz");
    assert(product != nullptr);
    gactorio::ProductionTask task(product);
    assert(task.currentStep() != nullptr);
    assert(task.currentStep()->id() == "mixing_step");
    assert(task.currentStepInputs().size() == 1);
    assert(task.currentStepInputs()[0].itemId() == "ingredient");
    assert(task.currentStepOutputs().empty());

    task.advanceStep();
    assert(task.currentStepKind() == "flavoring");
    assert(task.currentStep()->id() == "flavoring_step");
    assert(task.currentStepInputs().size() == 1);
    assert(task.currentStepOutputs().size() == 1);
    assert(task.currentStepOutputs()[0].itemId.value() == "flavored_base");

    return 0;
}
