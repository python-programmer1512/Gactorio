#include "model/MachineStates.hpp"

#include "model/Machine.hpp"

namespace gactorio {

std::unique_ptr<MachineState> makeMachineState(MachineStatus status) {
    switch (status) {
    case MachineStatus::Working:
        return std::make_unique<WorkingState>();
    case MachineStatus::Paused:
        return std::make_unique<PausedState>();
    case MachineStatus::Broken:
        return std::make_unique<BrokenState>();
    case MachineStatus::Maintenance:
        return std::make_unique<MaintenanceState>();
    case MachineStatus::Idle:
    case MachineStatus::Blocked:
    default:
        return std::make_unique<IdleState>();
    }
}

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

void PausedState::update(Machine& machine, double deltaTime) {
    (void)machine;
    (void)deltaTime;
}

std::string PausedState::name() const {
    return "Paused";
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
