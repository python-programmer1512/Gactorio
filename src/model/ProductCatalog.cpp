#include "model/ProductCatalog.hpp"

#include "model/Product.hpp"

#include <utility>

namespace gactorio {

bool ProductCatalog::registerProduct(ProductDefinition definition) {
    const auto id = definition.id;
    if (id == 0 || find(id) != nullptr) {
        return false;
    }

    products_.emplace(id, std::make_shared<ProductDefinition>(std::move(definition)));
    return true;
}

const ProductDefinition* ProductCatalog::find(ProductId id) const {
    const auto found = products_.find(id);
    if (found == products_.end()) {
        return nullptr;
    }
    return found->second.get();
}

std::shared_ptr<Product> ProductCatalog::createProduct(ProductId id) const {
    const auto found = products_.find(id);
    if (found == products_.end()) {
        return nullptr;
    }
    return std::make_shared<Product>(found->second);
}

std::string ProductCatalog::displayName(ProductId id) const {
    const auto* definition = find(id);
    if (definition == nullptr) {
        return "Unknown Product";
    }
    return definition->displayName;
}

} // namespace gactorio
