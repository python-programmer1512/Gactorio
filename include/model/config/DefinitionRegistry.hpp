#pragma once

#include "model/config/FactoryConfig.hpp"

#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>

namespace gactorio::config_model {

class DefinitionRegistryError : public std::runtime_error {
public:
    explicit DefinitionRegistryError(const std::string& message);
};

class DefinitionRegistry {
public:
    explicit DefinitionRegistry(const FactoryConfig& config);

    const FactoryConfig& config() const;

    const ItemDefinition* findItem(std::string_view id) const;
    const ProductDefinition* findProduct(std::string_view id) const;
    const StationDefinition* findStation(std::string_view id) const;
    const RecipeDefinition* findRecipe(std::string_view id) const;
    const ProductionLineDefinition* findLine(std::string_view id) const;
    const ScenarioDefinition* findScenario(std::string_view id) const;

    const ItemDefinition& requireItem(std::string_view id) const;
    const ProductDefinition& requireProduct(std::string_view id) const;
    const StationDefinition& requireStation(std::string_view id) const;
    const RecipeDefinition& requireRecipe(std::string_view id) const;
    const ProductionLineDefinition& requireLine(std::string_view id) const;
    const ScenarioDefinition& requireScenario(std::string_view id) const;

    bool stationAcceptsStep(const StationDefinition& station, std::string_view stepKind) const;
    bool lineCanProcessRecipe(const ProductionLineDefinition& line, const RecipeDefinition& recipe) const;
    bool lineCanProcessRecipe(std::string_view lineId, std::string_view recipeId) const;

private:
    void buildIndexes();
    void validateReferences() const;

    const FactoryConfig& config_;
    std::unordered_map<std::string, const ItemDefinition*> items_;
    std::unordered_map<std::string, const ProductDefinition*> products_;
    std::unordered_map<std::string, const StationDefinition*> stations_;
    std::unordered_map<std::string, const RecipeDefinition*> recipes_;
    std::unordered_map<std::string, const ProductionLineDefinition*> lines_;
    std::unordered_map<std::string, const ScenarioDefinition*> scenarios_;
};

} // namespace gactorio::config_model
