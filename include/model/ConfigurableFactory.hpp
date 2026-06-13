#pragma once

#include "model/Factory.hpp"
#include "model/ProductCatalog.hpp"
#include "model/Recipe.hpp"

#include <map>
#include <memory>
#include <vector>

namespace gactorio {

class ConfigurableFactory final : public Factory {
public:
    ConfigurableFactory();

    void setProductDefinitions(std::vector<ProductDefinition> definitions);
    void addRecipe(Recipe recipe);
    const std::vector<Recipe>& recipes() const;
    std::shared_ptr<Product> createConfiguredProduct(ProductId id) const;

protected:
    std::shared_ptr<Product> createProductById(ProductId id) const override;

private:
    std::vector<ProductDefinition> productDefinitions_;
    std::map<ProductId, std::size_t> productIndex_;
    std::vector<Recipe> recipes_;
};

} // namespace gactorio
