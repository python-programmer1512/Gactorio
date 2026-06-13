#include "controller/FactoryController.hpp"
#include "model/ConfiguredStation.hpp"
#include "model/Factory.hpp"
#include "model/FactoryBuilder.hpp"
#include "model/config/FactoryRuntimeContext.hpp"

#include <cassert>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

namespace {

std::filesystem::path customConfigPath() {
    return std::filesystem::path("..") / "data" / "factory_config.custom_runtime.json";
}

int inventoryQty(const gactorio::Factory& factory, const std::string& id) {
    const auto found = factory.inventory().items().find(id);
    return found == factory.inventory().items().end() ? 0 : found->second;
}

int productQty(const gactorio::Factory& factory, const std::string& id) {
    const auto found = factory.inventory().products().find(id);
    return found == factory.inventory().products().end() ? 0 : found->second;
}

int snapshotInventoryQty(const gactorio::FactorySnapshot& snapshot, const std::string& id) {
    for (const auto& entry : snapshot.inventory().items()) {
        if (entry.id() == id) {
            return entry.quantity();
        }
    }
    return 0;
}

bool hasProductDefinition(
    const std::vector<gactorio::ProductDefinition>& definitions,
    const std::string& id) {
    for (const auto& definition : definitions) {
        if (definition.id == id) {
            return true;
        }
    }
    return false;
}

const gactorio::Machine* findStationByKind(
    const gactorio::ProductionLine& line,
    const std::string& stationKind) {
    for (const auto& machine : line.machines()) {
        if (machine->stationKind() == stationKind) {
            return machine.get();
        }
    }
    return nullptr;
}

void runUntilComplete(gactorio::Factory& factory, int ticks) {
    for (int i = 0; i < ticks; ++i) {
        factory.update(1.0);
    }
}

void verifiesRuntimeContextAndRegistry() {
    auto context = gactorio::config_model::FactoryRuntimeContext::loadFromFile(customConfigPath());
    const auto& registry = context.registry();

    assert(registry.findItem("citrus_extract") != nullptr);
    assert(registry.findItem("sweetener") != nullptr);
    assert(registry.findProduct("citrus_surge") != nullptr);
    assert(registry.findStation("flavor_station") != nullptr);
    assert(registry.findStation("cooling_station") != nullptr);
    assert(registry.findStation("flavor_station")->kind == "flavoring");
    assert(registry.findStation("cooling_station")->kind == "cooling");
    assert(registry.findRecipe("citrus_surge_recipe") != nullptr);
    assert(registry.findLine("citrus_line") != nullptr);
}

void verifiesFactoryBuilderProductionAndStations() {
    auto context = gactorio::config_model::FactoryRuntimeContext::loadFromFile(customConfigPath());
    auto factory = gactorio::FactoryBuilder::createFactory(context);
    assert(factory != nullptr);
    assert(factory->productionLines().size() == 1);

    const auto& line = factory->productionLines().front();
    assert(line.definitionId() == "citrus_line");
    assert(line.machines().size() == 5);

    const auto* flavoring = findStationByKind(line, "flavoring");
    const auto* cooling = findStationByKind(line, "cooling");
    assert(flavoring != nullptr);
    assert(cooling != nullptr);
    assert(flavoring->role() == gactorio::MachineRole::Unknown);
    assert(cooling->role() == gactorio::MachineRole::Unknown);
    assert(flavoring->processType() == gactorio::ProcessType::Unknown);
    assert(cooling->processType() == gactorio::ProcessType::Unknown);

    const auto* configuredFlavoring = dynamic_cast<const gactorio::ConfiguredStation*>(flavoring);
    const auto* configuredCooling = dynamic_cast<const gactorio::ConfiguredStation*>(cooling);
    assert(configuredFlavoring != nullptr);
    assert(configuredCooling != nullptr);
    assert(configuredFlavoring->stationDefinitionId() == "flavor_station");
    assert(configuredCooling->stationDefinitionId() == "cooling_station");

    factory->update(0.0);
    assert(inventoryQty(*factory, "ingredient") == 0);
    assert(inventoryQty(*factory, "water") == 0);

    runUntilComplete(*factory, 5);
    assert(productQty(*factory, "citrus_surge") == 1);
    assert(inventoryQty(*factory, "mixed_citrus_base") == 0);
    assert(inventoryQty(*factory, "flavored_citrus_base") == 0);
    assert(inventoryQty(*factory, "chilled_citrus_base") == 0);
    assert(inventoryQty(*factory, "bottled_citrus") == 0);
}

void verifiesControllerStringApi() {
    auto controller = gactorio::FactoryController::createFromConfigFile(customConfigPath());
    assert(controller.hasRuntimeContext());
    assert(hasProductDefinition(controller.availableProductDefinitions(), "citrus_surge"));

    assert(controller.enqueueAutoById("citrus_surge") == 1);
    const auto beforeRestock = snapshotInventoryQty(controller.getFactorySnapshot(), "citrus_extract");
    assert(controller.restockItemById("citrus_extract") == gactorio::FactoryCommandResult::Success);
    const auto afterRestock = snapshotInventoryQty(controller.getFactorySnapshot(), "citrus_extract");
    assert(afterRestock == beforeRestock + 3);
    assert(controller.restockItemById("mixed_citrus_base") == gactorio::FactoryCommandResult::InvalidRequest);
}

void verifiesMementoRestoreForJsonOnlyExtension() {
    auto context = gactorio::config_model::FactoryRuntimeContext::loadFromFile(customConfigPath());
    auto factory = gactorio::FactoryBuilder::createFactory(context);
    assert(factory != nullptr);

    factory->update(0.0);
    factory->update(1.0);
    assert(inventoryQty(*factory, "mixed_citrus_base") == 0);

    const auto memento = factory->createMemento();
    factory->update(1.0);
    factory->restoreFromMemento(memento);

    const auto& line = factory->productionLines().front();
    assert(line.definitionId() == "citrus_line");
    assert(findStationByKind(line, "flavoring") != nullptr);
    assert(findStationByKind(line, "cooling") != nullptr);

    runUntilComplete(*factory, 4);
    assert(productQty(*factory, "citrus_surge") == 1);
    assert(inventoryQty(*factory, "mixed_citrus_base") == 0);
    assert(inventoryQty(*factory, "flavored_citrus_base") == 0);
    assert(inventoryQty(*factory, "chilled_citrus_base") == 0);
    assert(inventoryQty(*factory, "bottled_citrus") == 0);
}

} // namespace

int main() {
    verifiesRuntimeContextAndRegistry();
    verifiesFactoryBuilderProductionAndStations();
    verifiesControllerStringApi();
    verifiesMementoRestoreForJsonOnlyExtension();
    return 0;
}
