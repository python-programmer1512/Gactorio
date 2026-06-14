#include "model/config/DefinitionRegistry.hpp"

#include <algorithm>
#include <unordered_set>

namespace gactorio::config_model {
namespace {

std::string asString(std::string_view value) {
    return std::string(value);
}

template <typename Definition>
void addToIndex(
    std::unordered_map<std::string, const Definition*>& index,
    const Definition& definition,
    const std::string& kind,
    const std::string& path) {
    if (definition.id.empty()) {
        throw DefinitionRegistryError("Empty " + kind + " id at " + path);
    }
    const auto inserted = index.emplace(definition.id, &definition);
    if (!inserted.second) {
        throw DefinitionRegistryError("Duplicate " + kind + " id: " + definition.id);
    }
}

template <typename Value>
bool containsString(const std::vector<Value>& values, std::string_view needle) {
    return std::any_of(values.begin(), values.end(), [needle](const Value& value) {
        return value == needle;
    });
}

} // namespace

DefinitionRegistryError::DefinitionRegistryError(const std::string& message)
    : std::runtime_error(message) {}

DefinitionRegistry::DefinitionRegistry(const FactoryConfig& config)
    : config_(config) {
    buildIndexes();
    validateReferences();
}

const FactoryConfig& DefinitionRegistry::config() const {
    return config_;
}

const ItemDefinition* DefinitionRegistry::findItem(std::string_view id) const {
    const auto it = items_.find(asString(id));
    return it == items_.end() ? nullptr : it->second;
}

const ProductDefinition* DefinitionRegistry::findProduct(std::string_view id) const {
    const auto it = products_.find(asString(id));
    return it == products_.end() ? nullptr : it->second;
}

const StationDefinition* DefinitionRegistry::findStation(std::string_view id) const {
    const auto it = stations_.find(asString(id));
    return it == stations_.end() ? nullptr : it->second;
}

const RecipeDefinition* DefinitionRegistry::findRecipe(std::string_view id) const {
    const auto it = recipes_.find(asString(id));
    return it == recipes_.end() ? nullptr : it->second;
}

const ProductionLineDefinition* DefinitionRegistry::findLine(std::string_view id) const {
    const auto it = lines_.find(asString(id));
    return it == lines_.end() ? nullptr : it->second;
}

const ScenarioDefinition* DefinitionRegistry::findScenario(std::string_view id) const {
    const auto it = scenarios_.find(asString(id));
    return it == scenarios_.end() ? nullptr : it->second;
}

const ItemDefinition& DefinitionRegistry::requireItem(std::string_view id) const {
    if (const auto* value = findItem(id)) {
        return *value;
    }
    throw DefinitionRegistryError("Unknown item id: " + asString(id));
}

const ProductDefinition& DefinitionRegistry::requireProduct(std::string_view id) const {
    if (const auto* value = findProduct(id)) {
        return *value;
    }
    throw DefinitionRegistryError("Unknown product id: " + asString(id));
}

const StationDefinition& DefinitionRegistry::requireStation(std::string_view id) const {
    if (const auto* value = findStation(id)) {
        return *value;
    }
    throw DefinitionRegistryError("Unknown station id: " + asString(id));
}

const RecipeDefinition& DefinitionRegistry::requireRecipe(std::string_view id) const {
    if (const auto* value = findRecipe(id)) {
        return *value;
    }
    throw DefinitionRegistryError("Unknown recipe id: " + asString(id));
}

const ProductionLineDefinition& DefinitionRegistry::requireLine(std::string_view id) const {
    if (const auto* value = findLine(id)) {
        return *value;
    }
    throw DefinitionRegistryError("Unknown line id: " + asString(id));
}

const ScenarioDefinition& DefinitionRegistry::requireScenario(std::string_view id) const {
    if (const auto* value = findScenario(id)) {
        return *value;
    }
    throw DefinitionRegistryError("Unknown scenario id: " + asString(id));
}

bool DefinitionRegistry::stationAcceptsStep(const StationDefinition& station, std::string_view stepKind) const {
    if (stepKind.empty()) {
        return false;
    }
    if (station.acceptedStepKinds.empty()) {
        return station.kind == stepKind;
    }
    return containsString(station.acceptedStepKinds, stepKind);
}

bool DefinitionRegistry::lineCanProcessRecipe(
    const ProductionLineDefinition& line,
    const RecipeDefinition& recipe) const {
    for (const auto& step : recipe.steps) {
        bool canProcessStep = false;
        for (const auto& stationId : line.stationIds) {
            const auto* station = findStation(stationId);
            if (station != nullptr && stationAcceptsStep(*station, step.stepKind)) {
                canProcessStep = true;
                break;
            }
        }
        if (!canProcessStep) {
            return false;
        }
    }
    return true;
}

bool DefinitionRegistry::lineCanProcessRecipe(std::string_view lineId, std::string_view recipeId) const {
    const auto* line = findLine(lineId);
    const auto* recipe = findRecipe(recipeId);
    return line != nullptr && recipe != nullptr && lineCanProcessRecipe(*line, *recipe);
}

void DefinitionRegistry::buildIndexes() {
    for (std::size_t i = 0; i < config_.items.size(); ++i) {
        addToIndex(items_, config_.items[i], "item", "items[" + std::to_string(i) + "]");
    }
    for (std::size_t i = 0; i < config_.products.size(); ++i) {
        addToIndex(products_, config_.products[i], "product", "products[" + std::to_string(i) + "]");
    }
    for (std::size_t i = 0; i < config_.stations.size(); ++i) {
        addToIndex(stations_, config_.stations[i], "station", "stations[" + std::to_string(i) + "]");
        if (config_.stations[i].kind.empty()) {
            throw DefinitionRegistryError("Empty station kind at stations[" + std::to_string(i) + "]");
        }
    }
    for (std::size_t i = 0; i < config_.recipes.size(); ++i) {
        addToIndex(recipes_, config_.recipes[i], "recipe", "recipes[" + std::to_string(i) + "]");
        if (config_.recipes[i].productId.empty()) {
            throw DefinitionRegistryError("Empty recipe product id at recipes[" + std::to_string(i) + "]");
        }
    }
    for (std::size_t i = 0; i < config_.lines.size(); ++i) {
        addToIndex(lines_, config_.lines[i], "line", "lines[" + std::to_string(i) + "]");
    }
    for (std::size_t i = 0; i < config_.scenarios.size(); ++i) {
        addToIndex(scenarios_, config_.scenarios[i], "scenario", "scenarios[" + std::to_string(i) + "]");
    }
}

void DefinitionRegistry::validateReferences() const {
    for (const auto& product : config_.products) {
        if (!product.defaultRecipeId.empty()) {
            const auto* recipe = findRecipe(product.defaultRecipeId);
            if (recipe == nullptr) {
                throw DefinitionRegistryError(
                    "Product " + product.id + " references unknown defaultRecipeId: " + product.defaultRecipeId);
            }
            if (recipe->productId != product.id) {
                throw DefinitionRegistryError(
                    "Product " + product.id + " defaultRecipeId points to recipe for another product: " + recipe->id);
            }
        }
    }

    for (std::size_t recipeIndex = 0; recipeIndex < config_.recipes.size(); ++recipeIndex) {
        const auto& recipe = config_.recipes[recipeIndex];
        if (findProduct(recipe.productId) == nullptr) {
            throw DefinitionRegistryError(
                "Recipe " + recipe.id + " references unknown productId: " + recipe.productId);
        }
        if (recipe.steps.empty()) {
            throw DefinitionRegistryError("Recipe " + recipe.id + " has no steps");
        }

        for (std::size_t stepIndex = 0; stepIndex < recipe.steps.size(); ++stepIndex) {
            const auto& step = recipe.steps[stepIndex];
            const std::string stepPath = "recipes[" + std::to_string(recipeIndex) + "].steps[" + std::to_string(stepIndex) + "]";
            if (step.id.empty()) {
                throw DefinitionRegistryError("Empty recipe step id at " + stepPath);
            }
            if (step.stepKind.empty()) {
                throw DefinitionRegistryError("Empty recipe step kind at " + stepPath);
            }
            for (const auto& input : step.inputs) {
                if (findItem(input.itemId) == nullptr) {
                    throw DefinitionRegistryError(
                        "Recipe " + recipe.id + " step " + step.id + " references unknown input itemId: " + input.itemId);
                }
            }
            for (const auto& output : step.outputs) {
                const bool hasItem = output.itemId.has_value();
                const bool hasProduct = output.productId.has_value();
                if (hasItem == hasProduct) {
                    throw DefinitionRegistryError(
                        "Recipe " + recipe.id + " step " + step.id + " output must contain exactly one of itemId or productId");
                }
                if (hasItem && findItem(*output.itemId) == nullptr) {
                    throw DefinitionRegistryError(
                        "Recipe " + recipe.id + " step " + step.id + " references unknown output itemId: " + *output.itemId);
                }
                if (hasProduct && findProduct(*output.productId) == nullptr) {
                    throw DefinitionRegistryError(
                        "Recipe " + recipe.id + " step " + step.id + " references unknown output productId: " + *output.productId);
                }
            }
        }
    }

    for (const auto& line : config_.lines) {
        if (line.stationIds.empty()) {
            throw DefinitionRegistryError("Line " + line.id + " has no stationIds");
        }
        if (line.recipeIds.empty()) {
            throw DefinitionRegistryError("Line " + line.id + " has no recipeIds");
        }
        for (const auto& stationId : line.stationIds) {
            if (findStation(stationId) == nullptr) {
                throw DefinitionRegistryError("Line " + line.id + " references unknown stationId: " + stationId);
            }
        }
        for (const auto& recipeId : line.recipeIds) {
            const auto* recipe = findRecipe(recipeId);
            if (recipe == nullptr) {
                throw DefinitionRegistryError("Line " + line.id + " references unknown recipeId: " + recipeId);
            }
            for (const auto& step : recipe->steps) {
                bool canProcessStep = false;
                for (const auto& stationId : line.stationIds) {
                    const auto& station = requireStation(stationId);
                    if (stationAcceptsStep(station, step.stepKind)) {
                        canProcessStep = true;
                        break;
                    }
                }
                if (!canProcessStep) {
                    throw DefinitionRegistryError(
                        "Line " + line.id + " cannot process recipe " + recipe->id + " step kind: " + step.stepKind);
                }
            }
        }
    }

    std::unordered_set<std::string> inventoryItems;
    for (const auto& seed : config_.initialInventory) {
        if (findItem(seed.itemId) == nullptr) {
            throw DefinitionRegistryError("initialInventory references unknown itemId: " + seed.itemId);
        }
        if (!inventoryItems.insert(seed.itemId).second) {
            throw DefinitionRegistryError("Duplicate initialInventory itemId: " + seed.itemId);
        }
    }

    for (const auto& task : config_.startupTasks) {
        const auto* line = findLine(task.lineId);
        if (line == nullptr) {
            throw DefinitionRegistryError("startupTask references unknown lineId: " + task.lineId);
        }
        const auto* recipe = findRecipe(task.recipeId);
        if (recipe == nullptr) {
            throw DefinitionRegistryError("startupTask references unknown recipeId: " + task.recipeId);
        }
        if (!containsString(line->recipeIds, task.recipeId)) {
            throw DefinitionRegistryError(
                "startupTask recipe " + task.recipeId + " is not allowed on line " + task.lineId);
        }
        if (task.quantity <= 0) {
            throw DefinitionRegistryError("startupTask quantity must be greater than zero for recipeId: " + task.recipeId);
        }
    }

    for (const auto& scenario : config_.scenarios) {
        for (const auto& stationId : scenario.targetStationIds) {
            if (findStation(stationId) == nullptr) {
                throw DefinitionRegistryError(
                    "Scenario " + scenario.id + " references unknown targetStationId: " + stationId);
            }
        }
        for (const auto& stationKind : scenario.targetStationKinds) {
            const bool foundKind = std::any_of(
                config_.stations.begin(),
                config_.stations.end(),
                [&stationKind](const StationDefinition& station) {
                    return station.kind == stationKind;
                });
            if (!foundKind) {
                throw DefinitionRegistryError(
                    "Scenario " + scenario.id + " targetStationKind does not match any station kind: " + stationKind);
            }
        }
    }
}

} // namespace gactorio::config_model
