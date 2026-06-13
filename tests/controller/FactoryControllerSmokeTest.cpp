#include "controller/FactoryController.hpp"
#include "controller/Controller.h"

#include <cassert>
#include <algorithm>
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

int countLogsContaining(const std::vector<gactorio::EventSnapshot>& logs, const std::string& text) {
    return static_cast<int>(std::count_if(
        logs.begin(),
        logs.end(),
        [&text](const gactorio::EventSnapshot& event) {
            return event.message().find(text) != std::string::npos;
        }));
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

    assert(controller.setLineScenarioById(1, "overflow")
               == gactorio::FactoryCommandResult::Success);
    assert(controller.setLineScenarioById(1, "rush-order")
               == gactorio::FactoryCommandResult::InvalidRequest);
    assert(controller.setLineScenario(999, gactorio::ScenarioType::Bottleneck)
               == gactorio::FactoryCommandResult::NotFound);
    const auto scenario = controller.getLineScenario(1);
    assert(scenario.has_value());
    assert(*scenario == gactorio::ScenarioType::Overflow);
    assert(!controller.getLineScenario(999).has_value());
    assert(countLogsContaining(controller.getEventLogs(), "scenario changed to Overflow") >= 1);

    const auto logsBeforeDrop = controller.getEventLogs();
    const auto enqueuedBeforeDrop = countLogsContaining(logsBeforeDrop, "enqueued Aurora Zero");
    assert(controller.enqueueProductById(1, "aurora_zero")
               == gactorio::FactoryCommandResult::OverflowDropped);
    const auto logsAfterDrop = controller.getEventLogs();
    assert(countLogsContaining(logsAfterDrop, "overflow: product task dropped") >= 1);
    assert(countLogsContaining(logsAfterDrop, "enqueued Aurora Zero") == enqueuedBeforeDrop);

    auto snapshot = controller.getFactorySnapshot();
    assert(snapshot.productionLines().size() == 1);
    assert(snapshot.productionLines().front().queueLength() >= 1);
    assert(snapshot.productionLines().front().scenarioId() == "overflow");
    assert(snapshot.productionLines().front().scenarioName() == "Overflow");
    assert(snapshot.productionLines().front().queueCapacity() == 2);
    assert(snapshot.productionLines().front().droppedTaskCount() == 1);

    controller.tick(1.0);
    snapshot = controller.getFactorySnapshot();
    assert(snapshot.productionLines().front().currentTaskProgress() > 0.0);
    assert(snapshot.productionLines().front().currentTaskProgress() < 0.25);

    assert(controller.forceBreak(1)    == gactorio::FactoryCommandResult::Success);
    assert(controller.repairMachine(1) == gactorio::FactoryCommandResult::Success);
    assert(controller.forceBreak(999)  == gactorio::FactoryCommandResult::NotFound);

    const auto beforeRestock = controller.snapshot();
    const int ingredientsBefore = inventoryQty(beforeRestock, "ingredient");
    assert(controller.restockItem(gactorio::ItemType::Ingredient, 5)
               == gactorio::FactoryCommandResult::Success);
    assert(controller.restockItem(static_cast<gactorio::ItemType>(101), 5)
               == gactorio::FactoryCommandResult::InvalidRequest);
    assert(inventoryQty(controller.snapshot(), "ingredient") == ingredientsBefore + 5);

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

    ctrl::Controller viewController;
    auto view = viewController.snapshot();
    assert(!view.lines.empty());
    assert(view.lines.front().scenarioId == "normal-flow");
    assert(view.lines.front().scenarioName == "Normal Flow");
    assert(view.lines.front().queueCapacity == 0);
    assert(view.lines.front().droppedTaskCount == 0);

    assert(viewController.setLineScenario(1, "random-breakdowns"));
    assert(viewController.getLineScenario(1) == "random-breakdowns");
    view = viewController.snapshot();
    assert(view.lines.front().scenarioId == "random-breakdowns");
    assert(view.lines.front().scenarioName == "Random Breakdowns");
    assert(!viewController.setLineScenario(1, "rush-order"));
    assert(viewController.getLineScenario(999).empty());

    assert(viewController.setLineScenario(1, "overflow"));
    assert(viewController.enqueueProduct(1, 102));
    assert(!viewController.enqueueProduct(1, 103));
    view = viewController.snapshot();
    assert(view.lines.front().queueCapacity == 2);
    assert(view.lines.front().droppedTaskCount == 1);

    return 0;
}
