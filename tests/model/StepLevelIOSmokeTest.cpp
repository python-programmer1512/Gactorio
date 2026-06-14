#include "model/Factory.hpp"
#include "model/FactoryBuilder.hpp"
#include "model/config/FactoryRuntimeContext.hpp"

#include <cassert>
#include <memory>
#include <string>

namespace {

std::string stepIoJson(int flavorSyrupQuantity) {
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
    { "itemId": "ingredient", "quantity": 1 },
    { "itemId": "flavor_syrup", "quantity": )json" + std::to_string(flavorSyrupQuantity) + R"json( }
  ],
  "startupTasks": [
    { "lineId": "line_a", "recipeId": "flavored_voltz_recipe", "quantity": 1 }
  ]
}
)json";
}

int productQuantity(const gactorio::Factory& factory, const std::string& productId) {
    const auto found = factory.inventory().products().find(productId);
    return found == factory.inventory().products().end() ? 0 : found->second;
}

} // namespace

int main() {
    auto context = gactorio::config_model::FactoryRuntimeContext::loadFromString(stepIoJson(1));
    auto factory = gactorio::FactoryBuilder::createFactory(context);
    assert(factory != nullptr);
    assert(factory->inventory().getItemQuantity("ingredient") == 1);
    assert(factory->inventory().getItemQuantity("flavor_syrup") == 1);
    assert(factory->inventory().getItemQuantity("mixed_base") == 0);
    assert(productQuantity(*factory, "flavored_voltz") == 0);

    factory->update(0.0);
    assert(factory->inventory().getItemQuantity("ingredient") == 0);
    assert(factory->inventory().getItemQuantity("flavor_syrup") == 1);
    assert(factory->inventory().getItemQuantity("mixed_base") == 0);

    factory->update(1.0);
    assert(factory->inventory().getItemQuantity("ingredient") == 0);
    assert(factory->inventory().getItemQuantity("flavor_syrup") == 0);
    assert(factory->inventory().getItemQuantity("mixed_base") == 0);
    assert(productQuantity(*factory, "flavored_voltz") == 0);

    factory->update(1.0);
    assert(productQuantity(*factory, "flavored_voltz") == 1);
    assert(factory->productionLines().front().queueLength() == 0);

    auto blockedContext = gactorio::config_model::FactoryRuntimeContext::loadFromString(stepIoJson(0));
    auto blockedFactory = gactorio::FactoryBuilder::createFactory(blockedContext);
    assert(blockedFactory != nullptr);

    blockedFactory->update(0.0);
    assert(blockedFactory->inventory().getItemQuantity("ingredient") == 0);
    blockedFactory->update(1.0);
    assert(blockedFactory->inventory().getItemQuantity("mixed_base") == 1);
    assert(blockedFactory->inventory().getItemQuantity("flavor_syrup") == 0);
    assert(productQuantity(*blockedFactory, "flavored_voltz") == 0);
    assert(blockedFactory->productionLines().front().queueLength() == 1);
    assert(blockedFactory->productionLines().front().machines()[1]->currentTask() == nullptr);

    blockedFactory->inventory().addItem("flavor_syrup", 1);
    blockedFactory->update(0.0);
    assert(blockedFactory->inventory().getItemQuantity("mixed_base") == 0);
    assert(blockedFactory->inventory().getItemQuantity("flavor_syrup") == 0);
    assert(blockedFactory->productionLines().front().machines()[1]->currentTask() != nullptr);

    blockedFactory->update(1.0);
    assert(productQuantity(*blockedFactory, "flavored_voltz") == 1);
    assert(blockedFactory->productionLines().front().queueLength() == 0);

    return 0;
}
