#include "model/MachineStates.hpp"

#include "model/Machine.hpp"

namespace gactorio {

void IdleState::update(Machine& machine, double deltaTime) {
    (void)deltaTime;
    if (machine.hasTask() || machine.recipe().has_value()) {
        machine.transitionToWorking("task available");
    }
}

std::string IdleState::name() const {
    return "Idle";
}

void WorkingState::update(Machine& machine, double deltaTime) {
    machine.advanceProduction(deltaTime);
}

std::string WorkingState::name() const {
    return "Working";
}

void BrokenState::update(Machine& machine, double deltaTime) {
    (void)machine;
    (void)deltaTime;
}

std::string BrokenState::name() const {
    return "Broken";
}

void MaintenanceState::update(Machine& machine, double deltaTime) {
    machine.advanceMaintenance(deltaTime);
}

std::string MaintenanceState::name() const {
    return "Maintenance";
}

} // namespace gactorio
