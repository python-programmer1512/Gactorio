#include "model/ConfigurableFactory.hpp"

#include "model/StationFactory.hpp"

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

void ConfigurableFactory::setRuntimeContext(
    const config_model::FactoryRuntimeContext* context) noexcept {
    runtimeContext_ = context;
}

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

std::optional<ProductionLine> ConfigurableFactory::createLineForMemento(
    const LineMemento& memento) const {
    if (runtimeContext_ == nullptr || memento.definitionId().empty()) {
        return std::nullopt;
    }

    const auto* lineDefinition = runtimeContext_->registry().findLine(memento.definitionId());
    if (lineDefinition == nullptr) {
        return std::nullopt;
    }

    const auto displayName = memento.displayName().empty()
        ? (lineDefinition->displayName.empty() ? lineDefinition->id : lineDefinition->displayName)
        : memento.displayName();
    ProductionLine line(memento.id(), displayName);
    line.setDefinitionId(lineDefinition->id);

    for (std::size_t i = 0; i < memento.machines().size(); ++i) {
        const auto& machineMemento = memento.machines()[i];

        const config_model::StationDefinition* stationDefinition = nullptr;
        if (!machineMemento.stationDefinitionId().empty()) {
            stationDefinition = runtimeContext_->registry().findStation(
                machineMemento.stationDefinitionId());
        }
        if (stationDefinition == nullptr && i < lineDefinition->stationIds.size()) {
            stationDefinition = runtimeContext_->registry().findStation(lineDefinition->stationIds[i]);
        }
        if (stationDefinition == nullptr) {
            return std::nullopt;
        }

        line.addMachine(StationFactory::create(machineMemento.id(), *stationDefinition));
    }

    if (memento.queueCapacity().has_value()) {
        line.setQueueCapacity(*memento.queueCapacity());
    } else if (lineDefinition->queueCapacity.has_value()) {
        line.setQueueCapacity(*lineDefinition->queueCapacity);
    }
    return line;
}

} // namespace gactorio
