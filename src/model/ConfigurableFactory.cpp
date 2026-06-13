#include "model/ConfigurableFactory.hpp"

#include <memory>
#include <utility>

namespace gactorio {
namespace {

class ConfigProduct final : public Product {
public:
    explicit ConfigProduct(const ProductDefinition& definition)
        : Product(definition) {}

    ProductId getProductId() const override { return storedProductId(); }
    const ProductId& productId() const override { return storedProductIdRef(); }
    const RecipeId& defaultRecipeId() const override { return storedDefaultRecipeId(); }
    const std::string& getName() const override { return storedName(); }
    const std::vector<ItemRequirement>& getRequirements() const override { return storedRequirements(); }
    const std::vector<ProcessStep>& getRoute() const override { return storedRoute(); }
};

} // namespace

ConfigurableFactory::ConfigurableFactory() = default;

void ConfigurableFactory::setProductDefinitions(std::vector<ProductDefinition> definitions) {
    productDefinitions_ = std::move(definitions);
    productIndex_.clear();
    for (std::size_t i = 0; i < productDefinitions_.size(); ++i) {
        productIndex_[productDefinitions_[i].id] = i;
    }
}

void ConfigurableFactory::addRecipe(Recipe recipe) {
    recipes_.push_back(std::move(recipe));
}

const std::vector<Recipe>& ConfigurableFactory::recipes() const {
    return recipes_;
}

std::shared_ptr<Product> ConfigurableFactory::createConfiguredProduct(ProductId id) const {
    return createProductById(std::move(id));
}

std::shared_ptr<Product> ConfigurableFactory::createProductById(ProductId id) const {
    const auto found = productIndex_.find(id);
    if (found == productIndex_.end()) {
        return nullptr;
    }
    return std::make_shared<ConfigProduct>(productDefinitions_[found->second]);
}

} // namespace gactorio
