#pragma once

// =============================================================================
// Concrete States of the State pattern — each is-a MachineState (▷) and is a
// friend of Machine so update() can call the private transitionTo*() helpers.
// See Machine_State_Diagram and BE_Overall_Class_Diagram.
// =============================================================================

#include "model/MachineState.hpp"

namespace gactorio {

class IdleState final : public MachineState {
public:
    void update(Machine& machine, double deltaTime) override;
    std::string name() const override;
};

class WorkingState final : public MachineState {
public:
    void update(Machine& machine, double deltaTime) override;
    std::string name() const override;
};

class BrokenState final : public MachineState {
public:
    void update(Machine& machine, double deltaTime) override;
    std::string name() const override;
};

class MaintenanceState final : public MachineState {
public:
    void update(Machine& machine, double deltaTime) override;
    std::string name() const override;
};

} // namespace gactorio
