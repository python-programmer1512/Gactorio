#include "controller/FactoryController.hpp"
#include "model/CarbonationFactory.hpp"
#include "model/Inventory.hpp"
#include "model/ProductionTask.hpp"
#include "model/events/EventLogObserver.hpp"
#include "model/events/StatisticsObserver.hpp"

#include <cassert>
#include <memory>
#include <string>
#include <type_traits>

namespace {

int inventoryQuantity(const gactorio::FactorySnapshot& snapshot, const std::string& name) {
    for (const auto& item : snapshot.inventory().items()) {
        if (item.name() == name) {
            return item.quantity();
        }
    }
    return 0;
}

template <typename T, typename = void>
struct HasCreateMemento : std::false_type {};

template <typename T>
struct HasCreateMemento<T, std::void_t<decltype(&T::createMemento)>> : std::true_type {};

template <typename T, typename = void>
struct HasRestoreFromMemento : std::false_type {};

template <typename T>
struct HasRestoreFromMemento<T, std::void_t<decltype(&T::restoreFromMemento)>> : std::true_type {};

template <typename T, typename = void>
struct HasExportState : std::false_type {};

template <typename T>
struct HasExportState<T, std::void_t<decltype(&T::exportState)>> : std::true_type {};

template <typename T, typename = void>
struct HasRestoreState : std::false_type {};

template <typename T>
struct HasRestoreState<T, std::void_t<decltype(&T::restoreState)>> : std::true_type {};

} // namespace

int main() {
    static_assert(HasCreateMemento<gactorio::Factory>::value);
    static_assert(HasRestoreFromMemento<gactorio::Factory>::value);
    static_assert(!HasCreateMemento<gactorio::Inventory>::value);
    static_assert(!HasRestoreFromMemento<gactorio::Inventory>::value);
    static_assert(HasExportState<gactorio::Inventory>::value);
    static_assert(HasRestoreState<gactorio::Inventory>::value);

    gactorio::FactoryController controller;
    controller.clearHistory();

    auto snapshot = controller.getFactorySnapshot();
    const auto initialQueueLength = snapshot.productionLines().front().queueLength();
    const auto initialWater = inventoryQuantity(snapshot, "Water");

    assert(controller.enqueueProduct(1, gactorio::ProductType::SodaCan) == gactorio::FactoryCommandResult::Success);
    snapshot = controller.getFactorySnapshot();
    assert(snapshot.productionLines().front().queueLength() == initialQueueLength + 1);
    assert(inventoryQuantity(snapshot, "Water") == initialWater - 1);

    assert(controller.undo() == gactorio::FactoryCommandResult::Success);
    snapshot = controller.getFactorySnapshot();
    assert(snapshot.productionLines().front().queueLength() == initialQueueLength);
    assert(inventoryQuantity(snapshot, "Water") == initialWater);

    assert(controller.redo() == gactorio::FactoryCommandResult::Success);
    snapshot = controller.getFactorySnapshot();
    assert(snapshot.productionLines().front().queueLength() == initialQueueLength + 1);
    assert(inventoryQuantity(snapshot, "Water") == initialWater - 1);

    assert(controller.forceBreak(1) == gactorio::FactoryCommandResult::Success);
    snapshot = controller.getFactorySnapshot();
    assert(snapshot.productionLines().front().machines().front().status() == gactorio::MachineStatus::Broken);
    assert(controller.undo() == gactorio::FactoryCommandResult::Success);
    snapshot = controller.getFactorySnapshot();
    assert(snapshot.productionLines().front().machines().front().status() != gactorio::MachineStatus::Broken);

    assert(controller.forceBreak(1) == gactorio::FactoryCommandResult::Success);
    assert(controller.repairMachine(1) == gactorio::FactoryCommandResult::Success);
    snapshot = controller.getFactorySnapshot();
    assert(snapshot.productionLines().front().machines().front().status() == gactorio::MachineStatus::Maintenance);
    assert(controller.undo() == gactorio::FactoryCommandResult::Success);
    snapshot = controller.getFactorySnapshot();
    assert(snapshot.productionLines().front().machines().front().status() == gactorio::MachineStatus::Broken);
    assert(controller.redo() == gactorio::FactoryCommandResult::Success);
    snapshot = controller.getFactorySnapshot();
    assert(snapshot.productionLines().front().machines().front().status() == gactorio::MachineStatus::Maintenance);

    controller.reset();
    controller.clearHistory();
    assert(controller.saveState() == gactorio::FactoryCommandResult::Success);
    controller.tick(2.0);
    snapshot = controller.getFactorySnapshot();
    assert(snapshot.simulationTime() == 2.0);
    assert(controller.undo() == gactorio::FactoryCommandResult::Success);
    snapshot = controller.getFactorySnapshot();
    assert(snapshot.simulationTime() == 0.0);

    controller.clearHistory();
    controller.tick(1.0);
    assert(!controller.canUndo());

    assert(controller.saveState() == gactorio::FactoryCommandResult::Success);
    controller.tick(1.0);
    assert(controller.undo() == gactorio::FactoryCommandResult::Success);
    assert(controller.canRedo());
    controller.pauseSimulation();
    assert(!controller.canRedo());

    controller.clearHistory();
    snapshot = controller.getFactorySnapshot();
    const auto queueBeforeReset = snapshot.productionLines().front().queueLength();
    assert(controller.enqueueProduct(1, gactorio::ProductType::SodaCan) == gactorio::FactoryCommandResult::Success);
    controller.clearHistory();
    controller.reset();
    assert(controller.canUndo());
    snapshot = controller.getFactorySnapshot();
    assert(snapshot.productionLines().front().queueLength() == 1);
    assert(controller.undo() == gactorio::FactoryCommandResult::Success);
    snapshot = controller.getFactorySnapshot();
    assert(snapshot.productionLines().front().queueLength() == queueBeforeReset + 1);

    controller.reset();
    controller.clearHistory();
    const auto logsBefore = controller.getEventLogs().size();
    const auto statsBefore = controller.getStatistics().startedTaskEvents();
    assert(controller.saveState() == gactorio::FactoryCommandResult::Success);
    controller.tick(0.0);
    assert(controller.getEventLogs().size() > logsBefore);
    assert(controller.getStatistics().startedTaskEvents() > statsBefore);
    assert(controller.undo() == gactorio::FactoryCommandResult::Success);
    assert(controller.getEventLogs().size() == logsBefore);
    assert(controller.getStatistics().startedTaskEvents() == statsBefore);
    assert(controller.redo() == gactorio::FactoryCommandResult::Success);
    assert(controller.getEventLogs().size() > logsBefore);
    assert(controller.getStatistics().startedTaskEvents() > statsBefore);

    gactorio::CarbonationFactory factory;
    auto* machine = factory.findMachine(1);
    assert(machine != nullptr);
    machine->setRecipe(factory.recipes().front());
    const auto directState = factory.createMemento();
    assert(directState.productionLines.front().machines.front().recipe.has_value());
    assert(directState.productionLines.front().machines.front().recipe->id == factory.recipes().front().id());

    for (int i = 0; i < 10; ++i) {
        factory.update(10.0);
    }
    factory.restoreFromMemento(directState);
    assert(factory.machines().size() == 4);
    assert(factory.findMachine(1) != nullptr);
    assert(factory.findMachine(1)->recipe().has_value());
    assert(factory.findMachine(1)->recipe()->id() == factory.recipes().front().id());

    const auto eventCountAfterRestore = factory.eventLog().events().size();
    assert(factory.findMachine(1)->getStatus() == gactorio::MachineStatus::Idle);
    factory.findMachine(1)->restoreStateObject(gactorio::MachineStatus::Maintenance);
    const auto maintenanceState = factory.createMemento();
    factory.findMachine(1)->restoreStateObject(gactorio::MachineStatus::Idle);
    factory.restoreFromMemento(maintenanceState);
    assert(factory.findMachine(1)->getStatus() == gactorio::MachineStatus::Maintenance);
    assert(factory.findMachine(1)->stateName() == "Maintenance");
    assert(factory.eventLog().events().size() == eventCountAfterRestore);

    factory.restoreFromMemento(directState);
    factory.update(0.0);
    assert(!factory.eventLog().events().empty());
    assert(factory.statistics().startedTaskEvents() > directState.statistics.startedTaskEvents);
    const auto currentTask = factory.productionLines().front().currentTask();
    assert(currentTask != nullptr);
    factory.update(3.0);
    assert(factory.productionLines().front().currentTask() == currentTask);
    assert(currentTask->currentStepIndex() == 1);

    return 0;
}
