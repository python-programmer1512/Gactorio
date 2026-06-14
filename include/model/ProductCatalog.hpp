#pragma once

#include "model/Product.hpp"

#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace gactorio {
namespace config_model {
class DefinitionRegistry;
}

// Central product registry.
//
// UML dependency notes:
// - ProductCatalog -> ProductDefinition: composition (owns immutable catalog data).
// - ProductCatalog -> Product: dependency (factory methods create Product objects).
// - Controller/Factory should depend on this registry instead of switching on
//   every concrete product class.
struct ProductDefinition {
    ProductId id;
    ProductType type;
    RecipeId defaultRecipeId;
    std::string key;
    std::string name;
    std::string tier;
    double totalDurationSeconds;
    std::vector<ItemRequirement> requirements;
    std::vector<ProcessStep> route;
};

const std::vector<ProductDefinition>& productDefinitions();
const ProductDefinition* findProductDefinition(std::string_view id);
const ProductDefinition* findProductDefinition(ProductType type);

std::shared_ptr<Product> createProduct(std::string_view id);
std::shared_ptr<Product> createProduct(ProductType type);

std::vector<ProductDefinition> productDefinitionsFromRegistry(
    const config_model::DefinitionRegistry& registry);

std::optional<ProductDefinition> makeProductDefinitionFromRegistry(
    const config_model::DefinitionRegistry& registry,
    std::string_view productId);

std::shared_ptr<Product> createProductFromRegistry(
    const config_model::DefinitionRegistry& registry,
    std::string_view productId);

} // namespace gactorio
