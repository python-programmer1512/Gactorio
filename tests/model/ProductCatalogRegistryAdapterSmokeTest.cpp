#include "model/ProductCatalog.hpp"
#include "model/config/DefinitionRegistry.hpp"
#include "model/config/FactoryConfigLoader.hpp"

#include <algorithm>
#include <cassert>
#include <stdexcept>
#include <string>

namespace {

const gactorio::ProductDefinition* findDefinition(
    const std::vector<gactorio::ProductDefinition>& definitions,
    const std::string& id) {
    const auto found = std::find_if(
        definitions.begin(),
        definitions.end(),
        [&id](const gactorio::ProductDefinition& definition) {
            return definition.id == id;
        });
    return found == definitions.end() ? nullptr : &(*found);
}

int requiredQuantity(
    const gactorio::ProductDefinition& definition,
    const std::string& itemId) {
    const auto found = std::find_if(
        definition.requirements.begin(),
        definition.requirements.end(),
        [&itemId](const gactorio::ItemRequirement& requirement) {
            return requirement.itemId() == itemId;
        });
    return found == definition.requirements.end() ? 0 : found->quantity();
}

} // namespace

int main() {
    const std::string json = R"json(
{
  "schemaVersion": "1.0.0",
  "items": [
    { "id": "ingredient", "displayName": "Ingredient" },
    { "id": "water", "displayName": "Water" },
    { "id": "empty_bottle", "displayName": "Empty Bottle" },
    { "id": "label", "displayName": "Label" },
    { "id": "package", "displayName": "Package" }
  ],
  "products": [
    {
      "id": "voltz_classic",
      "displayName": "Voltz Classic",
      "tier": "standard",
      "defaultRecipeId": "voltz_classic_recipe"
    },
    {
      "id": "hyper_bolt",
      "displayName": "Hyper Bolt",
      "tier": "premium",
      "defaultRecipeId": "hyper_bolt_recipe"
    },
    {
      "id": "aurora_zero",
      "displayName": "Aurora Zero",
      "tier": "specialty",
      "defaultRecipeId": "aurora_zero_recipe"
    }
  ],
  "stations": [
    { "id": "mixing_station", "kind": "mixing", "displayName": "Mixer" },
    { "id": "quality_station", "kind": "quality", "displayName": "Quality Check" },
    { "id": "bottling_station", "kind": "bottling", "displayName": "Filler" },
    { "id": "packaging_station", "kind": "packaging", "displayName": "Packager" }
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
            { "itemId": "ingredient", "quantity": 2 },
            { "itemId": "water", "quantity": 1 }
          ]
        },
        { "id": "quality", "stepKind": "quality", "duration": 9.0 },
        {
          "id": "bottling",
          "stepKind": "bottling",
          "duration": 9.0,
          "inputs": [
            { "itemId": "empty_bottle", "quantity": 1 }
          ]
        },
        {
          "id": "packaging",
          "stepKind": "packaging",
          "duration": 9.0,
          "inputs": [
            { "itemId": "label", "quantity": 1 },
            { "itemId": "package", "quantity": 1 }
          ],
          "outputs": [
            { "productId": "voltz_classic", "quantity": 1 }
          ]
        }
      ]
    },
    {
      "id": "hyper_bolt_recipe",
      "productId": "hyper_bolt",
      "displayName": "Hyper Bolt Recipe",
      "steps": [
        {
          "id": "mixing",
          "stepKind": "mixing",
          "duration": 18.0,
          "inputs": [
            { "itemId": "ingredient", "quantity": 3 },
            { "itemId": "water", "quantity": 1 }
          ]
        },
        { "id": "quality", "stepKind": "quality", "duration": 12.0 },
        {
          "id": "bottling",
          "stepKind": "bottling",
          "duration": 9.0,
          "inputs": [
            { "itemId": "empty_bottle", "quantity": 1 }
          ]
        },
        {
          "id": "packaging",
          "stepKind": "packaging",
          "duration": 9.0,
          "inputs": [
            { "itemId": "label", "quantity": 1 },
            { "itemId": "package", "quantity": 1 }
          ],
          "outputs": [
            { "productId": "hyper_bolt", "quantity": 1 }
          ]
        }
      ]
    },
    {
      "id": "aurora_zero_recipe",
      "productId": "aurora_zero",
      "displayName": "Aurora Zero Recipe",
      "steps": [
        {
          "id": "mixing",
          "stepKind": "mixing",
          "duration": 17.0,
          "inputs": [
            { "itemId": "ingredient", "quantity": 2 },
            { "itemId": "water", "quantity": 1 }
          ]
        },
        { "id": "quality", "stepKind": "quality", "duration": 12.0 },
        {
          "id": "bottling",
          "stepKind": "bottling",
          "duration": 9.0,
          "inputs": [
            { "itemId": "empty_bottle", "quantity": 1 }
          ]
        },
        {
          "id": "packaging",
          "stepKind": "packaging",
          "duration": 11.0,
          "inputs": [
            { "itemId": "label", "quantity": 1 },
            { "itemId": "package", "quantity": 1 }
          ],
          "outputs": [
            { "productId": "aurora_zero", "quantity": 1 }
          ]
        }
      ]
    }
  ],
  "lines": [
    {
      "id": "line_a",
      "displayName": "Beverage Line A",
      "stationIds": [
        "mixing_station",
        "quality_station",
        "bottling_station",
        "packaging_station"
      ],
      "recipeIds": [
        "voltz_classic_recipe",
        "hyper_bolt_recipe",
        "aurora_zero_recipe"
      ]
    }
  ]
}
)json";

    const auto config = gactorio::config_model::FactoryConfigLoader::loadFromString(json);
    const gactorio::config_model::DefinitionRegistry registry(config);

    const auto definitions = gactorio::productDefinitionsFromRegistry(registry);
    assert(definitions.size() == 3);

    const auto* voltz = findDefinition(definitions, "voltz_classic");
    assert(voltz != nullptr);
    assert(voltz->defaultRecipeId == "voltz_classic_recipe");
    assert(voltz->name == "Voltz Classic");
    assert(voltz->tier == "standard");
    assert(voltz->totalDurationSeconds == 40.0);
    assert(voltz->route.size() == 4);
    assert(voltz->route[0].requiredRole() == gactorio::MachineRole::Mixing);
    assert(voltz->route[3].requiredRole() == gactorio::MachineRole::Packaging);
    assert(voltz->route[0].baseDurationSeconds() == 13.0);
    assert(voltz->route[3].baseDurationSeconds() == 9.0);
    assert(requiredQuantity(*voltz, "ingredient") == 2);
    assert(requiredQuantity(*voltz, "water") == 1);
    assert(requiredQuantity(*voltz, "empty_bottle") == 1);
    assert(requiredQuantity(*voltz, "label") == 1);
    assert(requiredQuantity(*voltz, "package") == 1);

    const auto hyper = gactorio::makeProductDefinitionFromRegistry(registry, "hyper_bolt");
    assert(hyper.has_value());
    assert(hyper->defaultRecipeId == "hyper_bolt_recipe");
    assert(requiredQuantity(*hyper, "ingredient") == 3);
    assert(hyper->route[0].baseDurationSeconds() == 18.0);

    const auto product = gactorio::createProductFromRegistry(registry, "voltz_classic");
    assert(product != nullptr);
    assert(product->productId() == "voltz_classic");
    assert(product->defaultRecipeId() == "voltz_classic_recipe");
    assert(product->getRequirements().size() == 5);
    assert(product->getRoute().size() == 4);

    assert(!gactorio::makeProductDefinitionFromRegistry(registry, "missing_product").has_value());
    assert(gactorio::createProductFromRegistry(registry, "missing_product") == nullptr);

    const std::string unsupportedStepJson = R"json(
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
      "id": "cooling_station",
      "kind": "cooling",
      "displayName": "Cooling",
      "acceptedStepKinds": ["cooling"]
    }
  ],
  "recipes": [
    {
      "id": "experimental_recipe",
      "productId": "experimental",
      "displayName": "Experimental Recipe",
      "steps": [
        {
          "id": "cooling",
          "stepKind": "cooling",
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
      "stationIds": ["cooling_station"],
      "recipeIds": ["experimental_recipe"]
    }
  ]
}
)json";

    const auto unsupportedConfig =
        gactorio::config_model::FactoryConfigLoader::loadFromString(unsupportedStepJson);
    const gactorio::config_model::DefinitionRegistry unsupportedRegistry(unsupportedConfig);
    bool sawUnsupportedStep = false;
    try {
        (void)gactorio::makeProductDefinitionFromRegistry(unsupportedRegistry, "experimental");
    } catch (const std::invalid_argument&) {
        sawUnsupportedStep = true;
    }
    assert(sawUnsupportedStep);

    return 0;
}
