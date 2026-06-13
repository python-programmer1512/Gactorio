#pragma once

#include <cstddef>
#include <optional>
#include <string>
#include <vector>

namespace gactorio::config_model {

using ItemId = std::string;
using ProductId = std::string;
using StationId = std::string;
using StationKind = std::string;
using StepKind = std::string;
using RecipeId = std::string;
using LineId = std::string;
using ScenarioId = std::string;

struct FactorySettings {
    double initialHealth = 100.0;
    double damageChancePerSecond = 0.0;
    double damageMinHp = 0.0;
    double damageMaxHp = 0.0;
    double incrementalRepairHp = 0.0;
    double repairAllDelaySeconds = 0.0;
    ScenarioId defaultLineScenarioId;
    double defaultSpeedMultiplier = 1.0;
};

struct ItemDefinition {
    ItemId id;
    std::string displayName;
    std::string category;
    bool restockable = false;
    int restockAmount = 0;
};

struct ProductDefinition {
    ProductId id;
    std::string displayName;
    std::string description;
    std::string tier;
    std::string color;
    int sortOrder = 0;
    RecipeId defaultRecipeId;
};

struct StationDefinition {
    StationId id;
    StationKind kind;
    std::string displayName;
    std::string typeName;
    std::vector<StepKind> acceptedStepKinds;
    double processingSpeed = 1.0;
    double maxHealth = 100.0;
    double breakdownProbability = 0.0;
};

struct ItemStackDefinition {
    ItemId itemId;
    int quantity = 0;
};

struct ProductStackDefinition {
    ProductId productId;
    int quantity = 0;
};

struct RecipeOutputDefinition {
    std::optional<ItemId> itemId;
    std::optional<ProductId> productId;
    int quantity = 0;
};

struct RecipeStepDefinition {
    std::string id;
    StepKind stepKind;
    double duration = 0.0;
    std::vector<ItemStackDefinition> inputs;
    std::vector<RecipeOutputDefinition> outputs;
};

struct RecipeDefinition {
    RecipeId id;
    ProductId productId;
    std::string displayName;
    std::vector<RecipeStepDefinition> steps;
};

struct ProductionLineDefinition {
    LineId id;
    std::string displayName;
    std::vector<StationId> stationIds;
    std::vector<RecipeId> recipeIds;
    std::optional<std::size_t> queueCapacity;
};

struct InventorySeed {
    ItemId itemId;
    int quantity = 0;
};

struct StartupTaskDefinition {
    LineId lineId;
    RecipeId recipeId;
    int quantity = 1;
};

struct ScenarioDefinition {
    ScenarioId id;
    std::string displayName;
    std::string description;
    std::vector<StationId> targetStationIds;
    std::vector<StationKind> targetStationKinds;
    std::optional<double> speedMultiplier;
    std::optional<double> breakdownProbabilityOverride;
    std::optional<int> queueCapacity;
};

struct FactoryConfig {
    std::string schemaVersion;
    FactorySettings settings;
    std::vector<ItemDefinition> items;
    std::vector<ProductDefinition> products;
    std::vector<StationDefinition> stations;
    std::vector<RecipeDefinition> recipes;
    std::vector<ProductionLineDefinition> lines;
    std::vector<InventorySeed> initialInventory;
    std::vector<StartupTaskDefinition> startupTasks;
    std::vector<ScenarioDefinition> scenarios;
};

} // namespace gactorio::config_model
