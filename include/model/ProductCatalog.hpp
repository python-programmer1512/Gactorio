#pragma once

#include "common/Types.hpp"
#include "model/ProductDefinition.hpp"

#include <map>
#include <memory>
#include <string>

namespace gactorio {

class Product;

class ProductCatalog {
public:
    bool registerProduct(ProductDefinition definition);

    const ProductDefinition* find(ProductId id) const;
    std::shared_ptr<Product> createProduct(ProductId id) const;
    std::string displayName(ProductId id) const;

private:
    std::map<ProductId, std::shared_ptr<const ProductDefinition>> products_;
};

} // namespace gactorio
