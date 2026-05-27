#include "controller/FactoryController.hpp"

#include <cassert>
#include <string>
#include <vector>

int main() {
    gactorio::FactoryController controller;

    controller.pauseSimulation();
    controller.startSimulation();
    controller.setSpeed(2.0);

    assert(controller.canUndo());
    assert(!controller.canRedo());

    auto snapshot = controller.getFactorySnapshot();
    const auto queueLengthBeforeFirstSuccess = snapshot.productionLines().front().queueLength();
    assert(controller.enqueueProduct(1, gactorio::ProductType::SparklingWater) == gactorio::FactoryCommandResult::Success);
    assert(controller.canUndo());
    assert(controller.undo() == gactorio::FactoryCommandResult::Success);
    snapshot = controller.getFactorySnapshot();
    assert(snapshot.productionLines().front().queueLength() == queueLengthBeforeFirstSuccess);
    assert(controller.canRedo());
    auto historyStatus = controller.getHistoryStatus();
    assert(historyStatus.canUndo());
    assert(historyStatus.canRedo());
    assert(controller.redo() == gactorio::FactoryCommandResult::Success);
    snapshot = controller.getFactorySnapshot();
    assert(snapshot.productionLines().front().queueLength() == queueLengthBeforeFirstSuccess + 1);
    assert(!controller.canRedo());

    assert(controller.enqueueProduct(999, gactorio::ProductType::SodaCan) == gactorio::FactoryCommandResult::NotFound);
    assert(controller.enqueueProduct(1, gactorio::ProductType::Unknown) == gactorio::FactoryCommandResult::UnknownProduct);

    const auto logsAfterFirstSuccess = controller.getEventLogs();
    std::vector<gactorio::EventType> enqueueEvents;
    for (const auto& event : logsAfterFirstSuccess) {
        if (event.type() == gactorio::EventType::InputsConsumed ||
            event.type() == gactorio::EventType::TaskEnqueued) {
            enqueueEvents.push_back(event.type());
        }
    }
    assert(enqueueEvents.size() >= 2);
    assert(enqueueEvents[enqueueEvents.size() - 2] == gactorio::EventType::InputsConsumed);
    assert(enqueueEvents[enqueueEvents.size() - 1] == gactorio::EventType::TaskEnqueued);
    assert(logsAfterFirstSuccess[logsAfterFirstSuccess.size() - 2].sourceId() == 1);
    assert(logsAfterFirstSuccess[logsAfterFirstSuccess.size() - 1].sourceId() == 0);

    for (int i = 0; i < 98; ++i) {
        assert(controller.enqueueProduct(1, gactorio::ProductType::SodaCan)
                   == gactorio::FactoryCommandResult::Success);
    }

    snapshot = controller.getFactorySnapshot();
    bool sawWaterInventory = false;
    for (const auto& entry : snapshot.inventory().items()) {
        if (entry.name() == "Water") {
            sawWaterInventory = true;
            assert(entry.quantity() == 0);
        }
    }
    assert(sawWaterInventory);

    const auto queueLengthBeforeShortage = snapshot.productionLines().front().queueLength();
    assert(controller.enqueueProduct(1, gactorio::ProductType::EnergyDrink)
               == gactorio::FactoryCommandResult::InsufficientMaterials);
    snapshot = controller.getFactorySnapshot();
    assert(snapshot.productionLines().front().queueLength() == queueLengthBeforeShortage);
    const auto logsAfterShortage = controller.getEventLogs();
    assert(!logsAfterShortage.empty());
    assert(logsAfterShortage.back().type() == gactorio::EventType::Info);

    assert(snapshot.productionLines().size() == 1);
    const auto& lineSnapshot = snapshot.productionLines().front();
    assert(lineSnapshot.name() == "Beverage Line");
    assert(lineSnapshot.queueLength() >= 1);
    assert(lineSnapshot.machines().size() == 4);
    assert(lineSnapshot.machines()[0].typeName() == "Carbonator");
    assert(lineSnapshot.machines()[1].typeName() == "Filler");
    assert(lineSnapshot.machines()[2].typeName() == "Sealer");
    assert(lineSnapshot.machines()[3].typeName() == "Labeler");

    assert(controller.forceBreak(1) == gactorio::FactoryCommandResult::Success);
    assert(controller.repairMachine(1) == gactorio::FactoryCommandResult::Success);
    assert(controller.forceBreak(999) == gactorio::FactoryCommandResult::NotFound);

    controller.tick(2.0);

    const auto logs = controller.getEventLogs();
    const auto stats = controller.getStatistics();

    assert(!logs.empty());
    assert(stats.stateChangedEvents() > 0);

    controller.resetSimulation();
    snapshot = controller.getFactorySnapshot();
    assert(snapshot.simulationTime() == 0.0);

    controller.clearHistory();
    assert(!controller.canUndo());
    assert(!controller.canRedo());
    assert(controller.enqueueProduct(999, gactorio::ProductType::SodaCan) == gactorio::FactoryCommandResult::NotFound);
    assert(controller.forceBreak(999) == gactorio::FactoryCommandResult::NotFound);
    assert(!controller.canUndo());
    assert(!controller.canRedo());

    assert(controller.enqueueProduct(1, gactorio::ProductType::SparklingWater) == gactorio::FactoryCommandResult::Success);
    snapshot = controller.getFactorySnapshot();
    const auto queueLengthBeforeReset = snapshot.productionLines().front().queueLength();
    controller.clearHistory();
    controller.reset();
    assert(controller.canUndo());
    snapshot = controller.getFactorySnapshot();
    assert(snapshot.productionLines().front().queueLength() == 1);
    assert(controller.undo() == gactorio::FactoryCommandResult::Success);
    snapshot = controller.getFactorySnapshot();
    assert(snapshot.productionLines().front().queueLength() == queueLengthBeforeReset);

    controller.clearHistory();
    assert(controller.saveState() == gactorio::FactoryCommandResult::Success);
    controller.tick(2.0);
    snapshot = controller.getFactorySnapshot();
    assert(snapshot.simulationTime() == 2.0);
    assert(controller.undo() == gactorio::FactoryCommandResult::Success);
    snapshot = controller.getFactorySnapshot();
    assert(snapshot.simulationTime() == 0.0);
    assert(controller.canRedo());
    controller.pauseSimulation();
    assert(!controller.canRedo());

    controller.reset();
    controller.clearHistory();
    assert(controller.saveState() == gactorio::FactoryCommandResult::Success);
    controller.tick(2.0);
    assert(controller.undo() == gactorio::FactoryCommandResult::Success);
    assert(controller.redo() == gactorio::FactoryCommandResult::Success);
    snapshot = controller.getFactorySnapshot();
    assert(snapshot.simulationTime() == 2.0);

    return 0;
}
