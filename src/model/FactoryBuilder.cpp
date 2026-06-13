#include "model/FactoryBuilder.hpp"

#include "model/ConfigurableFactory.hpp"
#include "model/ProductCatalog.hpp"
#include "model/StationFactory.hpp"

#include <map>
#include <stdexcept>
#include <utility>

namespace gactorio {
namespace {

double totalDuration(const config_model::RecipeDefinition& recipeDefinition) {
    double total = 0.0;
    for (const auto& step : recipeDefinition.steps) {
        total += step.duration;
    }
    return total;
}

Recipe makeRecipe(const config_model::RecipeDefinition& recipeDefinition) {
    Recipe recipe(
        recipeDefinition.id,
        recipeDefinition.displayName.empty() ? recipeDefinition.id : recipeDefinition.displayName,
        totalDuration(recipeDefinition));

    std::map<std::string, int> inputTotals;
    for (const auto& step : recipeDefinition.steps) {
        for (const auto& input : step.inputs) {
            inputTotals[input.itemId] += input.quantity;
        }
        for (const auto& output : step.outputs) {
            if (output.productId.has_value()) {
                recipe.addOutput(*output.productId, output.quantity);
            }
        }
    }

    for (const auto& input : inputTotals) {
        recipe.addInput(input.first, input.second);
    }
    return recipe;
}

std::string lineName(const config_model::ProductionLineDefinition& lineDefinition) {
    return lineDefinition.displayName.empty() ? lineDefinition.id : lineDefinition.displayName;
}

std::unique_ptr<Factory> buildFactory(
    const config_model::FactoryRuntimeContext& context,
    bool attachRuntimeContext) {
    const auto& config = context.config();
    const auto& registry = context.registry();

    auto factory = std::make_unique<ConfigurableFactory>();
    if (attachRuntimeContext) {
        factory->setRuntimeContext(&context);
    }
    factory->setProductDefinitions(productDefinitionsFromRegistry(registry));

    for (const auto& seed : config.initialInventory) {
        factory->inventory().addItem(seed.itemId, seed.quantity);
    }

    for (const auto& recipeDefinition : config.recipes) {
        factory->addRecipe(makeRecipe(recipeDefinition));
    }

    std::map<std::string, LineId> lineIds;
    LineId nextLineId = 1;
    MachineId nextMachineId = 1;
    for (const auto& lineDefinition : config.lines) {
        const LineId runtimeLineId = nextLineId++;
        lineIds[lineDefinition.id] = runtimeLineId;

        ProductionLine line(runtimeLineId, lineName(lineDefinition));
        line.setDefinitionId(lineDefinition.id);
        for (const auto& stationId : lineDefinition.stationIds) {
            const auto& stationDefinition = registry.requireStation(stationId);
            line.addMachine(StationFactory::create(nextMachineId++, stationDefinition));
        }
        if (lineDefinition.queueCapacity.has_value()) {
            line.setQueueCapacity(*lineDefinition.queueCapacity);
        }

        factory->addProductionLine(std::move(line));
    }

    for (const auto& startupTask : config.startupTasks) {
        const auto lineFound = lineIds.find(startupTask.lineId);
        if (lineFound == lineIds.end()) {
            throw std::invalid_argument("Unknown startupTask lineId: " + startupTask.lineId);
        }
        const auto& recipe = registry.requireRecipe(startupTask.recipeId);
        auto* line = factory->findProductionLine(lineFound->second);
        if (line == nullptr) {
            throw std::invalid_argument("Unable to find runtime line for startupTask lineId: " + startupTask.lineId);
        }
        for (int i = 0; i < startupTask.quantity; ++i) {
            auto product = factory->createConfiguredProduct(recipe.productId);
            if (product == nullptr) {
                throw std::invalid_argument("Unable to create product for startupTask recipeId: " + startupTask.recipeId);
            }
            const auto result = line->enqueueProduct(std::move(product));
            if (result == EnqueueResult::RejectedFull || result == EnqueueResult::LostOverflow) {
                throw std::invalid_argument("Unable to enqueue startupTask recipeId: " + startupTask.recipeId);
            }
        }
    }

    return factory;
}

} // namespace

std::unique_ptr<Factory> FactoryBuilder::createFactory(
    const config_model::FactoryRuntimeContext& context) {
    return buildFactory(context, true);
}

std::unique_ptr<Factory> FactoryBuilder::createFactoryFromConfigFile(
    const std::filesystem::path& path) {
    auto context = config_model::FactoryRuntimeContext::loadFromFile(path);
    return buildFactory(context, false);
}

std::unique_ptr<Factory> FactoryBuilder::createFactoryFromConfigString(
    std::string_view jsonText) {
    auto context = config_model::FactoryRuntimeContext::loadFromString(jsonText);
    return buildFactory(context, false);
}

} // namespace gactorio
