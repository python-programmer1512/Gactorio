#include "model/ProductionTask.hpp"

#include <cassert>
#include <string>

int main() {
    const gactorio::DroneFrame product;
    gactorio::ProductionTask task(product);

    assert(!task.isCompleted());
    assert(task.currentStepIndex() == 0);
    assert(task.currentStep() != nullptr);
    assert(task.currentStep()->requiredRole() == gactorio::MachineRole::Processor);
    assert(task.getProgressInRoute() == 0.0);
    assert(task.getProductName() == std::string("Drone Frame"));

    task.advanceStep();
    assert(!task.isCompleted());
    assert(task.currentStepIndex() == 1);
    assert(task.currentStep()->requiredRole() == gactorio::MachineRole::Producer);
    assert(task.getProgressInRoute() > 0.33);
    assert(task.getProgressInRoute() < 0.34);

    task.advanceStep();
    assert(!task.isCompleted());
    assert(task.currentStep()->requiredRole() == gactorio::MachineRole::Output);

    task.advanceStep();
    assert(task.isCompleted());
    assert(task.currentStep() == nullptr);
    assert(task.getProgressInRoute() == 1.0);

    task.advanceStep();
    assert(task.isCompleted());
    assert(task.getProgressInRoute() == 1.0);

    return 0;
}
