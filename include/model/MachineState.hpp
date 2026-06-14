#pragma once

// =============================================================================
// MachineState — abstract State role of the State pattern (GoF).
//
// UML references:
//   * Machine_State_Diagram      — the four runtime states and transitions.
//   * BE_Overall_Class_Diagram   — MachineState is abstract; IdleState /
//     WorkingState / BrokenState / MaintenanceState generalize it (▷).
//     Machine OWNS the current MachineState (composition ◆) and passes itself
//     (Context) into enter()/update()/exit() by reference (dependency).
// =============================================================================

#include <string>

namespace gactorio {

class Machine;

class MachineState {
public:
    virtual ~MachineState() = default;
    virtual void enter(Machine& machine);
    virtual void update(Machine& machine, double deltaTime) = 0;
    virtual void exit(Machine& machine);
    virtual std::string name() const = 0;
};

} // namespace gactorio

