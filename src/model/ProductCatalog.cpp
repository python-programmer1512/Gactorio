#include "model/ProductCatalog.hpp"

#include "common/Config.h"
#include "model/config/ConfigIdAdapters.hpp"
#include "model/config/DefinitionRegistry.hpp"

#include <algorithm>
#include <map>
#include <memory>
#include <stdexcept>
#include <utility>

namespace gactorio {

namespace {

class CatalogProduct final : public Product {
public:
    explicit CatalogProduct(const ProductDefinition& definition)
        : Product(definition) {}

    ProductId getProductId() const override { return storedProductId(); }
    const ProductId& productId() const override { return storedProductIdRef(); }
    const RecipeId& defaultRecipeId() const override { return storedDefaultRecipeId(); }
    const std::string& getName() const override { return storedName(); }
    const std::vector<ItemRequirement>& getRequirements() const override { return storedRequirements(); }
    const std::vector<ProcessStep>& getRoute() const override { return storedRoute(); }
};

RecipeId defaultRecipeIdFor(
    const config_model::DefinitionRegistry& registry,
    const config_model::ProductDefinition& product) {
    if (!product.defaultRecipeId.empty()) {
        return product.defaultRecipeId;
    }

    for (const auto& recipe : registry.config().recipes) {
        if (recipe.productId == product.id) {
            return recipe.id;
        }
    }
    return {};
}

std::vector<ItemRequirement> requirementsFromRecipe(
    const config_model::RecipeDefinition& recipe) {
    std::map<std::string, int> totals;
    for (const auto& step : recipe.steps) {
        for (const auto& input : step.inputs) {
            totals[input.itemId] += input.quantity;
        }
    }

    std::vector<ItemRequirement> requirements;
    requirements.reserve(totals.size());
    for (const auto& total : totals) {
        requirements.emplace_back(total.first, total.second);
    }
    return requirements;
}

std::vector<ProcessStep> routeFromRecipe(
    const config_model::RecipeDefinition& recipe) {
    std::vector<ProcessStep> route;
    route.reserve(recipe.steps.size());
    for (const auto& step : recipe.steps) {
        const auto role = config_model::machineRoleFromKind(step.stepKind);
        if (!role.has_value()) {
            throw std::invalid_argument(
                "Unsupported recipe step kind for runtime Product route: " + step.stepKind);
        }
        route.emplace_back(*role, step.duration);
    }
    return route;
}

double totalDurationFromRecipe(const config_model::RecipeDefinition& recipe) {
    double total = 0.0;
    for (const auto& step : recipe.steps) {
        total += step.duration;
    }
    return total;
}

void validateProductOutputs(
    const config_model::RecipeDefinition& recipe,
    const config_model::ProductDefinition& product) {
    for (const auto& step : recipe.steps) {
        for (const auto& output : step.outputs) {
            if (output.productId.has_value() && *output.productId != product.id) {
                throw std::invalid_argument(
                    "Recipe " + recipe.id + " outputs product " + *output.productId +
                    " while catalog product is " + product.id);
            }
        }
    }
}

} // namespace

const std::vector<ProductDefinition>& productDefinitions() {
    static const std::vector<ProductDefinition> definitions = {
        {
            "voltz_classic",
            ProductType::VoltzClassic,
            "voltz_classic_recipe",
            "VoltzClassic",
            "Voltz Classic",
            "standard",
            config::kProductVoltzClassicTotalTime,
            {
                ItemRequirement("ingredient",   2),
                ItemRequirement("water",        1),
                ItemRequirement("empty_bottle", 1),
                ItemRequirement("label",        1),
                ItemRequirement("package",      1),
            },
            {
                ProcessStep(MachineRole::Mixing,    config::kProductVoltzClassicMixingTime),
                ProcessStep(MachineRole::Quality,   config::kProductVoltzClassicQualityTime),
                ProcessStep(MachineRole::Bottling,  config::kProductVoltzClassicBottlingTime),
                ProcessStep(MachineRole::Packaging, config::kProductVoltzClassicPackagingTime),
            },
        },
        {
            "hyper_bolt",
            ProductType::HyperBolt,
            "hyper_bolt_recipe",
            "HyperBolt",
            "Hyper Bolt",
            "premium",
            config::kProductHyperBoltTotalTime,
            {
                ItemRequirement("ingredient",   3),
                ItemRequirement("water",        1),
                ItemRequirement("empty_bottle", 1),
                ItemRequirement("label",        1),
                ItemRequirement("package",      1),
            },
            {
                ProcessStep(MachineRole::Mixing,    config::kProductHyperBoltMixingTime),
                ProcessStep(MachineRole::Quality,   config::kProductHyperBoltQualityTime),
                ProcessStep(MachineRole::Bottling,  config::kProductHyperBoltBottlingTime),
                ProcessStep(MachineRole::Packaging, config::kProductHyperBoltPackagingTime),
            },
        },
        {
            "aurora_zero",
            ProductType::AuroraZero,
            "aurora_zero_recipe",
            "AuroraZero",
            "Aurora Zero",
            "specialty",
            config::kProductAuroraZeroTotalTime,
            {
                ItemRequirement("ingredient",   2),
                ItemRequirement("water",        1),
                ItemRequirement("empty_bottle", 1),
                ItemRequirement("label",        1),
                ItemRequirement("package",      1),
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

const ProductDefinition* findProductDefinition(std::string_view id) {
    const auto& definitions = productDefinitions();
    const auto it = std::find_if(
        definitions.begin(),
        definitions.end(),
        [id](const ProductDefinition& definition) {
            return definition.id == std::string(id);
        });
    return it == definitions.end() ? nullptr : &(*it);
}

const ProductDefinition* findProductDefinition(ProductType type) {
    if (type == ProductType::Unknown) {
        return nullptr;
    }
    return findProductDefinition(config_model::toProductId(type));
}

std::shared_ptr<Product> createProduct(std::string_view id) {
    const auto* definition = findProductDefinition(id);
    if (definition == nullptr) {
        return nullptr;
    }
    return std::make_shared<CatalogProduct>(*definition);
}

std::shared_ptr<Product> createProduct(ProductType type) {
    if (type == ProductType::Unknown) {
        return nullptr;
    }
    return createProduct(config_model::toProductId(type));
}

std::vector<ProductDefinition> productDefinitionsFromRegistry(
    const config_model::DefinitionRegistry& registry) {
    std::vector<ProductDefinition> definitions;
    definitions.reserve(registry.config().products.size());
    for (const auto& product : registry.config().products) {
        auto definition = makeProductDefinitionFromRegistry(registry, product.id);
        if (definition.has_value()) {
            definitions.push_back(std::move(*definition));
        }
    }
    return definitions;
}

std::optional<ProductDefinition> makeProductDefinitionFromRegistry(
    const config_model::DefinitionRegistry& registry,
    std::string_view productId) {
    const auto* configProduct = registry.findProduct(productId);
    if (configProduct == nullptr) {
        return std::nullopt;
    }

    const auto recipeId = defaultRecipeIdFor(registry, *configProduct);
    if (recipeId.empty()) {
        return std::nullopt;
    }

    const auto& recipe = registry.requireRecipe(recipeId);
    validateProductOutputs(recipe, *configProduct);

    ProductDefinition definition;
    definition.id = configProduct->id;
    definition.type = config_model::productTypeFromId(configProduct->id).value_or(ProductType::Unknown);
    definition.defaultRecipeId = recipe.id;
    definition.key = configProduct->id;
    definition.name = configProduct->displayName.empty() ? configProduct->id : configProduct->displayName;
    definition.tier = configProduct->tier;
    definition.totalDurationSeconds = totalDurationFromRecipe(recipe);
    definition.requirements = requirementsFromRecipe(recipe);
    definition.route = routeFromRecipe(recipe);
    return definition;
}

std::shared_ptr<Product> createProductFromRegistry(
    const config_model::DefinitionRegistry& registry,
    std::string_view productId) {
    auto definition = makeProductDefinitionFromRegistry(registry, productId);
    if (!definition.has_value()) {
        return nullptr;
    }
    return std::make_shared<CatalogProduct>(*definition);
}

} // namespace gactorio
