#include "model/Machine.hpp"
#include "model/DefaultProducts.hpp"
#include "model/ProductionTask.hpp"
#include "model/events/EventBus.hpp"
#include "model/events/EventLogObserver.hpp"

#include <cassert>
#include <memory>
#include <unordered_map>

int main() {
    constexpr gactorio::ProductId EnergyDrinkProductId = 103;

    gactorio::ProductCatalog catalog;
    gactorio::registerDefaultProducts(catalog);
    auto product = catalog.createProduct(EnergyDrinkProductId);
    assert(product != nullptr);
    auto task = std::make_shared<gactorio::ProductionTask>(product);
    gactorio::Carbonator machine(1, "Carbonator 1");

    assert(machine.getId() == 1);
    assert(machine.getName() == "Carbonator 1");
    assert(machine.getStatus() == gactorio::MachineStatus::Idle);
    assert(machine.getHealth() == 100.0);
    assert(machine.canAcceptTask());
    assert(machine.canProcess(gactorio::MachineRole::Carbonator));
    assert(!machine.canProcess(gactorio::MachineRole::Labeler));

    assert(machine.assignTask(task));
    assert(!machine.canAcceptTask());
    assert(machine.getStatus() == gactorio::MachineStatus::Working);

    machine.update(1.0);
    assert(machine.getProgress() > 0.08);
    assert(machine.getProgress() < 0.09);

    const auto snapshot = machine.getSnapshot();
    assert(snapshot.id() == machine.getId());
    assert(snapshot.status() == gactorio::MachineStatus::Working);
    assert(snapshot.health() == 100.0);

    std::unordered_map<const gactorio::ProductionTask*, gactorio::TaskMementoId> taskIds;
    taskIds.emplace(task.get(), 7);

    gactorio::Recipe recipe(12, "Machine Test Recipe", 3.0);
    recipe.addInput(gactorio::ItemType::Water, 1);
    recipe.addOutput(EnergyDrinkProductId, 1);
    machine.setRecipe(recipe);

    const auto savedState = machine.exportState(taskIds);
    assert(savedState.type == gactorio::MachineTypeKind::Carbonator);
    assert(savedState.id == 1);
    assert(savedState.name == "Carbonator 1");
    assert(savedState.status == gactorio::MachineStatus::Working);
    assert(savedState.rawProgress == 1.0);
    assert(savedState.health == 100.0);
    assert(savedState.processingSpeed == 1.0);
    assert(savedState.breakdownProbability == 0.01);
    assert(savedState.maintenanceElapsed == 0.0);
    assert(savedState.maintenanceDuration == 2.0);
    assert(savedState.simulationTime == 1.0);
    assert(savedState.recipe.has_value());
    assert(savedState.recipe->id == 12);
    assert(savedState.assignedTaskId.has_value());
    assert(*savedState.assignedTaskId == 7);

    const auto taskState = task->exportState(7);
    const auto restoredTask = gactorio::ProductionTask::fromState(taskState, catalog);
    assert(restoredTask != nullptr);

    std::unordered_map<gactorio::TaskMementoId, std::shared_ptr<gactorio::ProductionTask>> restoredTasks;
    restoredTasks.emplace(7, restoredTask);

    gactorio::EventBus bus;
    gactorio::EventLogObserver log;
    bus.subscribe(&log);

    gactorio::Carbonator restoredMachine(99, "Temporary Carbonator", 4.0, 12.0, 0.5);
    restoredMachine.setEventBus(&bus);
    restoredMachine.restoreState(savedState, restoredTasks);
    assert(log.events().empty());
    assert(restoredMachine.getId() == 1);
    assert(restoredMachine.getName() == "Carbonator 1");
    assert(restoredMachine.getStatus() == gactorio::MachineStatus::Working);
    assert(restoredMachine.stateName() == "Working");
    assert(restoredMachine.getHealth() == 100.0);
    assert(restoredMachine.getProcessingSpeed() == 1.0);
    assert(restoredMachine.getBreakdownProbability() == 0.01);
    assert(restoredMachine.hasTask());
    assert(restoredMachine.recipe().has_value());
    assert(restoredMachine.recipe()->id() == 12);

    restoredMachine.update(2.0);
    assert(restoredTask->currentStepIndex() == 1);

    machine.forceBreak();
    assert(machine.getStatus() == gactorio::MachineStatus::Broken);
    assert(machine.getHealth() == 0.0);
    assert(!machine.canAcceptTask());

    machine.repair();
    assert(machine.getStatus() == gactorio::MachineStatus::Maintenance);
    assert(machine.getHealth() == 0.0);
    assert(!machine.canAcceptTask());

    machine.update(2.0);
    assert(machine.getStatus() == gactorio::MachineStatus::Idle);
    assert(machine.getHealth() == 100.0);
    assert(machine.canAcceptTask());

    return 0;
}
