#include "model/Factory.hpp"
#include "model/FactoryBuilder.hpp"
#include "model/config/FactoryRuntimeContext.hpp"

#include <cassert>
#include <memory>
#include <string>

namespace {

std::string stepIoJson(int ingredientQuantity, int flavorSyrupQuantity) {
    return R"json(
{
  "schemaVersion": "1.0.0",
  "items": [
    { "id": "ingredient", "displayName": "Ingredient" },
    { "id": "mixed_base", "displayName": "Mixed Base" },
    { "id": "flavor_syrup", "displayName": "Flavor Syrup" }
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
    }
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
          "duration": 1.0,
          "inputs": [
            { "itemId": "ingredient", "quantity": 1 }
          ],
          "outputs": [
            { "itemId": "mixed_base", "quantity": 1 }
          ]
        },
        {
          "id": "flavoring_step",
          "stepKind": "flavoring",
          "duration": 1.0,
          "inputs": [
            { "itemId": "mixed_base", "quantity": 1 },
            { "itemId": "flavor_syrup", "quantity": 1 }
          ],
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
      "stationIds": ["mixing_station", "flavor_station"],
      "recipeIds": ["flavored_voltz_recipe"]
    }
  ],
  "initialInventory": [
    { "itemId": "ingredient", "quantity": )json" + std::to_string(ingredientQuantity) + R"json( },
    { "itemId": "flavor_syrup", "quantity": )json" + std::to_string(flavorSyrupQuantity) + R"json( }
  ],
  "startupTasks": [
    { "lineId": "line_a", "recipeId": "flavored_voltz_recipe", "quantity": 1 }
  ]
}
)json";
}

std::unique_ptr<gactorio::Factory> makeFactory(int ingredientQuantity, int flavorSyrupQuantity) {
    auto context = gactorio::config_model::FactoryRuntimeContext::loadFromString(
        stepIoJson(ingredientQuantity, flavorSyrupQuantity));
    auto factory = gactorio::FactoryBuilder::createFactory(context);
    assert(factory != nullptr);
    return factory;
}

int productQuantity(const gactorio::Factory& factory, const std::string& productId) {
    const auto found = factory.inventory().products().find(productId);
    return found == factory.inventory().products().end() ? 0 : found->second;
}

void consumedInputsAreNotConsumedAgainAfterRestore() {
    auto factory = makeFactory(1, 1);

    factory->update(0.0);
    assert(factory->inventory().getItemQuantity("ingredient") == 0);
    assert(factory->inventory().getItemQuantity("mixed_base") == 0);

    const auto memento = factory->createMemento();
    factory->restoreFromMemento(memento);
    assert(factory->inventory().getItemQuantity("ingredient") == 0);

    factory->update(0.0);
    assert(factory->inventory().getItemQuantity("ingredient") == 0);
    assert(factory->inventory().getItemQuantity("mixed_base") == 0);

    factory->update(1.0);
    assert(factory->inventory().getItemQuantity("mixed_base") == 0);
    assert(factory->inventory().getItemQuantity("flavor_syrup") == 0);
    assert(productQuantity(*factory, "flavored_voltz") == 0);

    factory->update(1.0);
    assert(productQuantity(*factory, "flavored_voltz") == 1);
    assert(factory->productionLines().front().queueLength() == 0);
}

void completedStepOutputsAreNotProducedAgainAfterRestore() {
    auto factory = makeFactory(1, 0);

    factory->update(0.0);
    factory->update(1.0);
    assert(factory->inventory().getItemQuantity("ingredient") == 0);
    assert(factory->inventory().getItemQuantity("mixed_base") == 1);
    assert(productQuantity(*factory, "flavored_voltz") == 0);

    const auto memento = factory->createMemento();
    factory->restoreFromMemento(memento);
    assert(factory->inventory().getItemQuantity("mixed_base") == 1);

    factory->update(0.0);
    assert(factory->inventory().getItemQuantity("mixed_base") == 1);
    assert(productQuantity(*factory, "flavored_voltz") == 0);

    factory->inventory().addItem("flavor_syrup", 1);
    factory->update(0.0);
    assert(factory->inventory().getItemQuantity("mixed_base") == 0);
    assert(factory->inventory().getItemQuantity("flavor_syrup") == 0);

    factory->update(1.0);
    assert(productQuantity(*factory, "flavored_voltz") == 1);
    assert(factory->productionLines().front().queueLength() == 0);
}

void queuedTaskCanConsumeInputsAfterRestore() {
    auto factory = makeFactory(0, 1);

    factory->update(0.0);
    assert(factory->inventory().getItemQuantity("ingredient") == 0);
    assert(factory->productionLines().front().queueLength() == 1);

    const auto memento = factory->createMemento();
    factory->restoreFromMemento(memento);
    factory->inventory().addItem("ingredient", 1);

    factory->update(0.0);
    assert(factory->inventory().getItemQuantity("ingredient") == 0);
    assert(factory->inventory().getItemQuantity("mixed_base") == 0);

    factory->update(1.0);
    factory->update(1.0);
    assert(productQuantity(*factory, "flavored_voltz") == 1);
}

} // namespace

int main() {
    consumedInputsAreNotConsumedAgainAfterRestore();
    completedStepOutputsAreNotProducedAgainAfterRestore();
    queuedTaskCanConsumeInputsAfterRestore();
    return 0;
}
