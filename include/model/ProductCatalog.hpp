#pragma once

#include "model/Product.hpp"

#include <memory>
#include <string>
#include <vector>

namespace gactorio {

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
    std::string key;
    std::string name;
    std::string tier;
    double totalDurationSeconds;
    std::vector<ItemRequirement> requirements;
    std::vector<ProcessStep> route;
};

const std::vector<ProductDefinition>& productDefinitions();
const ProductDefinition* findProductDefinition(ProductId id);
const ProductDefinition* findProductDefinition(ProductType type);

std::shared_ptr<Product> createProduct(ProductId id);
std::shared_ptr<Product> createProduct(ProductType type);

} // namespace gactorio
