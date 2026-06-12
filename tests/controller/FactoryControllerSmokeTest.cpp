#include "controller/FactoryController.hpp"

#include <cassert>
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

} // namespace

int main() {
    gactorio::FactoryController controller;

    controller.pauseSimulation();
    controller.startSimulation();
    controller.setSpeed(2.0);

    assert(controller.enqueueProduct(1,   gactorio::ProductType::HyperBolt)
               == gactorio::FactoryCommandResult::Success);
    assert(controller.enqueueProduct(999, gactorio::ProductType::VoltzClassic)
               == gactorio::FactoryCommandResult::NotFound);
    assert(controller.enqueueProduct(1,   gactorio::ProductType::Unknown)
               == gactorio::FactoryCommandResult::InvalidRequest);

    auto snapshot = controller.getFactorySnapshot();
    assert(snapshot.productionLines().size() == 1);
    assert(snapshot.productionLines().front().queueLength() >= 1);

    controller.tick(1.0);
    snapshot = controller.getFactorySnapshot();
    assert(snapshot.productionLines().front().currentTaskProgress() > 0.0);
    assert(snapshot.productionLines().front().currentTaskProgress() < 0.25);

    assert(controller.forceBreak(1)    == gactorio::FactoryCommandResult::Success);
    assert(controller.repairMachine(1) == gactorio::FactoryCommandResult::Success);
    assert(controller.forceBreak(999)  == gactorio::FactoryCommandResult::NotFound);

    const auto beforeRestock = controller.snapshot();
    const int ingredientsBefore = inventoryQty(beforeRestock, "1");
    assert(controller.restockItem(gactorio::ItemType::Ingredient, 5)
               == gactorio::FactoryCommandResult::Success);
    assert(controller.restockItem(static_cast<gactorio::ItemType>(101), 5)
               == gactorio::FactoryCommandResult::InvalidRequest);
    assert(inventoryQty(controller.snapshot(), "1") == ingredientsBefore + 5);

    controller.tick(2.0);

    const auto logs  = controller.getEventLogs();
    const auto stats = controller.getStatistics();

    assert(!logs.empty());
    assert(stats.stateChangedEvents() > 0);

    controller.resetSimulation();
    snapshot = controller.getFactorySnapshot();
    assert(snapshot.simulationTime() == 0.0);

    assert(controller.enqueueProduct(1, gactorio::ProductType::HyperBolt)
               == gactorio::FactoryCommandResult::Success);
    controller.tick(13.0);
    snapshot = controller.getFactorySnapshot();
    const auto& machines = snapshot.productionLines().front().machines();
    assert(machines[0].status() == gactorio::MachineStatus::Working);
    assert(machines[1].status() == gactorio::MachineStatus::Working);

    // The backend should never allow removing the last remaining line.
    for (int i = 0; i < 20; ++i) controller.tick(5.0);
    assert(controller.removeLine(1) == gactorio::FactoryCommandResult::InvalidRequest);

    return 0;
}
