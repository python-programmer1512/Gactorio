#include "model/ProductionTask.hpp"

#include <cassert>
#include <string>

int main() {
    // AuroraZero has a 4-step route: Mixing -> Quality -> Bottling -> Packaging
    const gactorio::AuroraZero product;
    gactorio::ProductionTask task(product);

    assert(!task.isCompleted());
    assert(task.currentStepIndex() == 0);
    assert(task.currentStep() != nullptr);
    assert(task.currentStep()->requiredRole() == gactorio::MachineRole::Mixing);
    assert(task.getProgressInRoute() == 0.0);
    assert(task.getProductName() == std::string("Aurora Zero"));

    task.advanceStep();
    assert(!task.isCompleted());
    assert(task.currentStepIndex() == 1);
    assert(task.currentStep()->requiredRole() == gactorio::MachineRole::Quality);
    assert(task.getProgressInRoute() > 0.24);
    assert(task.getProgressInRoute() < 0.26);

    task.advanceStep();
    assert(!task.isCompleted());
    assert(task.currentStep()->requiredRole() == gactorio::MachineRole::Bottling);

    task.advanceStep();
    assert(!task.isCompleted());
    assert(task.currentStep()->requiredRole() == gactorio::MachineRole::Packaging);

    task.advanceStep();
    assert(task.isCompleted());
    assert(task.currentStep() == nullptr);
    assert(task.getProgressInRoute() == 1.0);

    task.advanceStep();
    assert(task.isCompleted());
    assert(task.getProgressInRoute() == 1.0);

    return 0;
}
