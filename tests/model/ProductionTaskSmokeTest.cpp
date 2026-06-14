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
    assert(task.currentStepKind() == "mixing");
    assert(task.getProgressInRoute() == 0.0);
    assert(task.getProductName() == std::string("Aurora Zero"));
    assert(task.productId() == "aurora_zero");
    assert(task.recipeId() == "aurora_zero_recipe");

    task.advanceStep();
    assert(!task.isCompleted());
    assert(task.currentStepIndex() == 1);
    assert(task.currentStep()->requiredRole() == gactorio::MachineRole::Quality);
    assert(task.currentStepKind() == "quality");
    assert(task.getProgressInRoute() > 0.24);
    assert(task.getProgressInRoute() < 0.26);

    task.advanceStep();
    assert(!task.isCompleted());
    assert(task.currentStep()->requiredRole() == gactorio::MachineRole::Bottling);
    assert(task.currentStepKind() == "bottling");

    task.advanceStep();
    assert(!task.isCompleted());
    assert(task.currentStep()->requiredRole() == gactorio::MachineRole::Packaging);
    assert(task.currentStepKind() == "packaging");

    task.advanceStep();
    assert(task.isCompleted());
    assert(task.currentStep() == nullptr);
    assert(task.getProgressInRoute() == 1.0);

    task.advanceStep();
    assert(task.isCompleted());
    assert(task.getProgressInRoute() == 1.0);

    return 0;
}
