#pragma once

#include "model/Factory.hpp"
#include "model/ProductCatalog.hpp"
#include "model/Recipe.hpp"
#include "model/config/FactoryRuntimeContext.hpp"

#include <map>
#include <memory>
#include <optional>
#include <vector>

namespace gactorio {

class ConfigurableFactory final : public Factory {
public:
    ConfigurableFactory();

    void setRuntimeContext(const config_model::FactoryRuntimeContext* context) noexcept;
    void setProductDefinitions(std::vector<ProductDefinition> definitions);
    void addRecipe(Recipe recipe);
    const std::vector<Recipe>& recipes() const;
    std::shared_ptr<Product> createConfiguredProduct(ProductId id) const;

protected:
    std::shared_ptr<Product> createProductById(ProductId id) const override;
    std::optional<ProductionLine> createLineForMemento(const LineMemento& memento) const override;

private:
    const config_model::FactoryRuntimeContext* runtimeContext_ = nullptr;
    std::vector<ProductDefinition> productDefinitions_;
    std::map<ProductId, std::size_t> productIndex_;
    std::vector<Recipe> recipes_;
};

} // namespace gactorio
