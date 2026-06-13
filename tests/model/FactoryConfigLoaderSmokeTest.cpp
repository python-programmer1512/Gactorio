#include "model/config/FactoryConfigLoader.hpp"

#include <cassert>
#include <string>

int main() {
    const std::string json = R"json(
{
  "schemaVersion": "1.0.0",
  "settings": {
    "initialHealth": 100.0
  },
  "items": [
    {
      "id": "ingredient",
      "displayName": "Ingredient"
    }
  ],
  "products": [
    {
      "id": "voltz_classic",
      "displayName": "Voltz Classic"
    }
  ],
  "stations": [
    {
      "id": "mixing_station",
      "kind": "mixing",
      "displayName": "Mixer"
    }
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

    const auto config = gactorio::config_model::FactoryConfigLoader::loadFromString(json);
    assert(config.schemaVersion == "1.0.0");
    assert(config.settings.initialHealth == 100.0);
    assert(config.items.size() == 1);
    assert(config.items[0].id == "ingredient");
    assert(config.products.size() == 1);
    assert(config.stations.size() == 1);
    assert(config.recipes.size() == 1);
    assert(config.recipes[0].steps.size() == 1);
    assert(config.recipes[0].steps[0].inputs.size() == 1);
    assert(config.recipes[0].steps[0].outputs.size() == 1);
    assert(config.recipes[0].steps[0].outputs[0].productId.value() == "voltz_classic");
    assert(config.lines.size() == 1);

    bool sawExpectedError = false;
    try {
        (void)gactorio::config_model::FactoryConfigLoader::loadFromString(R"json(
{
  "schemaVersion": "1.0.0",
  "items": [],
  "products": [],
  "stations": [],
  "recipes": [
    {
      "id": "bad_recipe",
      "productId": "voltz_classic",
      "displayName": "Bad Recipe",
      "steps": [
        {
          "id": "bad_step",
          "stepKind": "mixing",
          "duration": 1.0,
          "outputs": [
            { "itemId": "mixed_base", "productId": "voltz_classic", "quantity": 1 }
          ]
        }
      ]
    }
  ],
  "lines": []
}
)json");
    } catch (const gactorio::config_model::FactoryConfigError&) {
        sawExpectedError = true;
    }
    assert(sawExpectedError);

    return 0;
}
