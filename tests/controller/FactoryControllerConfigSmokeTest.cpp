#include "controller/FactoryController.hpp"
#include "model/config/FactoryConfigLoader.hpp"

#include <cassert>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

namespace {

int inventoryQty(const gactorio::FactorySnapshot& snapshot, const std::string& id) {
    for (const auto& entry : snapshot.inventory().items()) {
        if (entry.id() == id) {
            return entry.quantity();
        }
    }
    return 0;
}

const gactorio::InventoryEntrySnapshot* findInventoryEntry(
    const gactorio::FactorySnapshot& snapshot,
    const std::string& id) {
    for (const auto& entry : snapshot.inventory().items()) {
        if (entry.id() == id) {
            return &entry;
        }
    }
    return nullptr;
}

std::string readTextFile(const std::filesystem::path& path) {
    std::ifstream file(path);
    assert(file.is_open());
    std::ostringstream out;
    out << file.rdbuf();
    return out.str();
}

} // namespace

int main() {
    const auto configPath =
        std::filesystem::path("..") / "data" / "factory_config.runtime.json";

    auto controller = gactorio::FactoryController::createFromConfigFile(configPath);
    assert(controller.hasRuntimeContext());
    assert(controller.config() != nullptr);
    assert(controller.registry() != nullptr);

    auto snapshot = controller.getFactorySnapshot();
    assert(snapshot.productionLines().size() == 1);
    assert(inventoryQty(snapshot, "ingredient") == 5);
    assert(inventoryQty(snapshot, "water") == 5);
    assert(inventoryQty(snapshot, "empty_bottle") == 5);
    assert(inventoryQty(snapshot, "label") == 5);
    assert(inventoryQty(snapshot, "package") == 5);
    const auto* ingredient = findInventoryEntry(snapshot, "ingredient");
    assert(ingredient != nullptr);
    assert(ingredient->displayName() == "Ingredient");
    assert(ingredient->kind() == "raw");
    assert(ingredient->restockable());
    assert(ingredient->restockAmount() == 5);

    const auto productDefinitions = controller.availableProductDefinitions();
    assert(productDefinitions.size() == 3);
    assert(productDefinitions.front().id == "voltz_classic");
    assert(productDefinitions.front().name == "Voltz Classic");

    const auto& line = snapshot.productionLines().front();
    assert(line.id() == 1);
    assert(line.name() == "Beverage Line A");
    assert(line.queueLength() == 1);
    assert(line.currentTaskName() == "Voltz Classic");
    assert(line.machines().size() == 4);
    assert(line.machines()[0].typeName() == "Mixing Station");

    controller.tick(0.1);
    snapshot = controller.getFactorySnapshot();
    assert(snapshot.productionLines().front().currentTaskProgress() >= 0.0);

    // After config load, line A already holds a queued Voltz Classic whose
    // mixing step consumes ingredient x2 (5 → 3) once the first tick assigns it.
    // Enqueuing hyper_bolt only queues it (no tick yet), so it has not consumed.
    assert(controller.enqueueProductById(1, "hyper_bolt")
           == gactorio::FactoryCommandResult::Success);
    assert(inventoryQty(controller.getFactorySnapshot(), "ingredient") == 3);

    // restockItemById adds restockAmount (5): 3 → 8.
    assert(controller.restockItemById("ingredient")
           == gactorio::FactoryCommandResult::Success);
    assert(inventoryQty(controller.getFactorySnapshot(), "ingredient") == 8);
    assert(controller.restockItemById("not_an_item")
           == gactorio::FactoryCommandResult::InvalidRequest);

    controller.saveCheckpoint();
    assert(controller.canUndo());
    controller.tick(0.1);
    assert(controller.undo());

    controller.reset();
    assert(controller.hasRuntimeContext());
    snapshot = controller.getFactorySnapshot();
    assert(snapshot.productionLines().size() == 1);
    assert(inventoryQty(snapshot, "ingredient") == 5);
    assert(snapshot.productionLines().front().currentTaskName() == "Voltz Classic");

    const auto configText = readTextFile(configPath);
    auto fromString = gactorio::FactoryController::createFromConfigString(configText);
    assert(fromString.hasRuntimeContext());
    assert(fromString.getFactorySnapshot().productionLines().size() == 1);

    gactorio::FactoryController defaultController;
    assert(!defaultController.hasRuntimeContext());
    assert(defaultController.availableProductDefinitions().size() == 3);

    bool sawMissingPath = false;
    try {
        (void)gactorio::FactoryController::createFromConfigFile(
            std::filesystem::path("..") / "data" / "missing_factory_config.json");
    } catch (const gactorio::config_model::FactoryConfigError&) {
        sawMissingPath = true;
    } catch (...) {
        sawMissingPath = true;
    }
    assert(sawMissingPath);

    return 0;
}
