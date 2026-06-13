#include "model/ProductCatalog.hpp"
#include "model/config/DefinitionRegistry.hpp"
#include "model/config/FactoryRuntimeContext.hpp"

#include <cassert>
#include <filesystem>
#include <string>

namespace {

const char* kMinimalJson = R"json(
{
  "schemaVersion": "1.0.0",
  "items": [
    { "id": "ingredient", "displayName": "Ingredient" }
  ],
  "products": [
    {
      "id": "voltz_classic",
      "displayName": "Voltz Classic",
      "defaultRecipeId": "voltz_classic_recipe"
    }
  ],
  "stations": [
    { "id": "mixing_station", "kind": "mixing", "displayName": "Mixer" }
  ],
  "recipes": [
    {
      "id": "voltz_classic_recipe",
      "productId": "voltz_classic",
      "displayName": "Voltz Classic Recipe",
      "steps": [
        {
          "id": "mixing",
          "stepKind": "mixing",
          "duration": 13.0,
          "inputs": [
            { "itemId": "ingredient", "quantity": 2 }
          ],
          "outputs": [
            { "productId": "voltz_classic", "quantity": 1 }
          ]
        }
      ]
    }
  ],
  "lines": [
    {
      "id": "line_a",
      "displayName": "Line A",
      "stationIds": ["mixing_station"],
      "recipeIds": ["voltz_classic_recipe"]
    }
  ]
}
)json";

const char* kInvalidJson = R"json(
{
  "schemaVersion": "1.0.0",
  "items": [
    { "id": "ingredient", "displayName": "Ingredient" }
  ],
  "products": [
    {
      "id": "voltz_classic",
      "displayName": "Voltz Classic",
      "defaultRecipeId": "voltz_classic_recipe"
    }
  ],
  "stations": [
    { "id": "mixing_station", "kind": "mixing", "displayName": "Mixer" }
  ],
  "recipes": [
    {
      "id": "voltz_classic_recipe",
      "productId": "voltz_classic",
      "displayName": "Voltz Classic Recipe",
      "steps": [
        {
          "id": "mixing",
          "stepKind": "mixing",
          "duration": 13.0,
          "inputs": [
            { "itemId": "missing_item", "quantity": 2 }
          ],
          "outputs": [
            { "productId": "voltz_classic", "quantity": 1 }
          ]
        }
      ]
    }
  ],
  "lines": [
    {
      "id": "line_a",
      "displayName": "Line A",
      "stationIds": ["mixing_station"],
      "recipeIds": ["voltz_classic_recipe"]
    }
  ]
}
)json";

} // namespace

int main() {
    auto context = gactorio::config_model::FactoryRuntimeContext::loadFromString(kMinimalJson);
    assert(context.config().schemaVersion == "1.0.0");
    assert(&context.registry().config() == &context.config());
    assert(context.registry().requireProduct("voltz_classic").displayName == "Voltz Classic");
    assert(context.registry().requireRecipe("voltz_classic_recipe").productId == "voltz_classic");
    assert(context.registry().lineCanProcessRecipe("line_a", "voltz_classic_recipe"));

    const auto definitions = gactorio::productDefinitionsFromRegistry(context.registry());
    assert(definitions.size() == 1);
    assert(definitions.front().id == "voltz_classic");
    assert(definitions.front().defaultRecipeId == "voltz_classic_recipe");

    const auto product = gactorio::createProductFromRegistry(context.registry(), "voltz_classic");
    assert(product != nullptr);
    assert(product->productId() == "voltz_classic");

    auto fileContext = gactorio::config_model::FactoryRuntimeContext::loadFromFile(
        std::filesystem::path("..") / "data" / "factory_config.runtime.json");
    assert(fileContext.config().products.size() == 3);
    assert(fileContext.registry().lineCanProcessRecipe("line_a", "aurora_zero_recipe"));

    bool sawRegistryError = false;
    try {
        (void)gactorio::config_model::FactoryRuntimeContext::loadFromString(kInvalidJson);
    } catch (const gactorio::config_model::DefinitionRegistryError&) {
        sawRegistryError = true;
    }
    assert(sawRegistryError);

    return 0;
}
