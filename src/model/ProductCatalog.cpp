#include "model/ProductCatalog.hpp"

#include "common/Config.h"

#include <algorithm>
#include <memory>

namespace gactorio {

namespace {

class CatalogProduct final : public Product {
public:
    explicit CatalogProduct(const ProductDefinition& definition)
        : Product(definition) {}

    ProductId getProductId() const override { return storedProductId(); }
    const std::string& getName() const override { return storedName(); }
    const std::vector<ItemRequirement>& getRequirements() const override { return storedRequirements(); }
    const std::vector<ProcessStep>& getRoute() const override { return storedRoute(); }
};

} // namespace

const std::vector<ProductDefinition>& productDefinitions() {
    static const std::vector<ProductDefinition> definitions = {
        {
            static_cast<ProductId>(ProductType::VoltzClassic),
            ProductType::VoltzClassic,
            "VoltzClassic",
            "Voltz Classic",
            "standard",
            config::kProductVoltzClassicTotalTime,
            {
                ItemRequirement(ItemType::Ingredient,  2),
                ItemRequirement(ItemType::Water,       1),
                ItemRequirement(ItemType::EmptyBottle, 1),
                ItemRequirement(ItemType::Label,       1),
                ItemRequirement(ItemType::Package,     1),
            },
            {
                ProcessStep(MachineRole::Mixing,    config::kProductVoltzClassicMixingTime),
                ProcessStep(MachineRole::Quality,   config::kProductVoltzClassicQualityTime),
                ProcessStep(MachineRole::Bottling,  config::kProductVoltzClassicBottlingTime),
                ProcessStep(MachineRole::Packaging, config::kProductVoltzClassicPackagingTime),
            },
        },
        {
            static_cast<ProductId>(ProductType::HyperBolt),
            ProductType::HyperBolt,
            "HyperBolt",
            "Hyper Bolt",
            "premium",
            config::kProductHyperBoltTotalTime,
            {
                ItemRequirement(ItemType::Ingredient,  3),
                ItemRequirement(ItemType::Water,       1),
                ItemRequirement(ItemType::EmptyBottle, 1),
                ItemRequirement(ItemType::Label,       1),
                ItemRequirement(ItemType::Package,     1),
            },
            {
                ProcessStep(MachineRole::Mixing,    config::kProductHyperBoltMixingTime),
                ProcessStep(MachineRole::Quality,   config::kProductHyperBoltQualityTime),
                ProcessStep(MachineRole::Bottling,  config::kProductHyperBoltBottlingTime),
                ProcessStep(MachineRole::Packaging, config::kProductHyperBoltPackagingTime),
            },
        },
        {
            static_cast<ProductId>(ProductType::AuroraZero),
            ProductType::AuroraZero,
            "AuroraZero",
            "Aurora Zero",
            "specialty",
            config::kProductAuroraZeroTotalTime,
            {
                ItemRequirement(ItemType::Ingredient,  2),
                ItemRequirement(ItemType::Water,       1),
                ItemRequirement(ItemType::EmptyBottle, 1),
                ItemRequirement(ItemType::Label,       1),
                ItemRequirement(ItemType::Package,     1),
            },
            {
                ProcessStep(MachineRole::Mixing,    config::kProductAuroraZeroMixingTime),
                ProcessStep(MachineRole::Quality,   config::kProductAuroraZeroQualityTime),
                ProcessStep(MachineRole::Bottling,  config::kProductAuroraZeroBottlingTime),
                ProcessStep(MachineRole::Packaging, config::kProductAuroraZeroPackagingTime),
            },
        },
    };

    return definitions;
}

const ProductDefinition* findProductDefinition(ProductId id) {
    const auto& definitions = productDefinitions();
    const auto it = std::find_if(
        definitions.begin(),
        definitions.end(),
        [id](const ProductDefinition& definition) {
            return definition.id == id;
        });
    return it == definitions.end() ? nullptr : &(*it);
}

const ProductDefinition* findProductDefinition(ProductType type) {
    return findProductDefinition(static_cast<ProductId>(type));
}

std::shared_ptr<Product> createProduct(ProductId id) {
    const auto* definition = findProductDefinition(id);
    if (definition == nullptr) {
        return nullptr;
    }
    return std::make_shared<CatalogProduct>(*definition);
}

std::shared_ptr<Product> createProduct(ProductType type) {
    return createProduct(static_cast<ProductId>(type));
}

} // namespace gactorio
