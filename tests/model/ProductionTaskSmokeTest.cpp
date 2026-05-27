#include "model/ProductionTask.hpp"

#include <cassert>
#include <string>

int main() {
    const gactorio::EnergyDrink product;
    gactorio::ProductionTask task(product);

    assert(!task.isCompleted());
    assert(task.currentStepIndex() == 0);
    assert(task.currentStep() != nullptr);
    assert(task.currentStep()->requiredRole() == gactorio::MachineRole::Carbonator);
    assert(task.getProgressInRoute() == 0.0);
    assert(task.getProductName() == std::string("Energy Drink"));

    task.advanceStep();
    assert(!task.isCompleted());
    assert(task.currentStepIndex() == 1);
    assert(task.currentStep()->requiredRole() == gactorio::MachineRole::Filler);
    assert(task.getProgressInRoute() == 0.25);

    const auto savedState = task.exportState(42);
    assert(savedState.taskId == 42);
    assert(savedState.productId == static_cast<gactorio::ProductId>(gactorio::ProductType::EnergyDrink));
    assert(savedState.currentStepIndex == 1);

    const auto restoredTask = gactorio::ProductionTask::fromState(savedState);
    assert(restoredTask != nullptr);
    assert(restoredTask->getProductName() == std::string("Energy Drink"));
    assert(restoredTask->currentStepIndex() == 1);
    assert(restoredTask->currentStep() != nullptr);
    assert(restoredTask->currentStep()->requiredRole() == gactorio::MachineRole::Filler);
    assert(restoredTask->getProgressInRoute() == 0.25);

    gactorio::ProductionTaskMemento unknownProductState;
    unknownProductState.taskId = 43;
    unknownProductState.productId = static_cast<gactorio::ProductId>(gactorio::ProductType::Unknown);
    assert(gactorio::ProductionTask::fromState(unknownProductState) == nullptr);

    task.advanceStep();
    assert(!task.isCompleted());
    assert(task.currentStep()->requiredRole() == gactorio::MachineRole::Sealer);

    task.advanceStep();
    assert(!task.isCompleted());
    assert(task.currentStep()->requiredRole() == gactorio::MachineRole::Labeler);

    task.advanceStep();
    assert(task.isCompleted());
    assert(task.currentStep() == nullptr);
    assert(task.getProgressInRoute() == 1.0);

    task.advanceStep();
    assert(task.isCompleted());
    assert(task.getProgressInRoute() == 1.0);

    return 0;
}
