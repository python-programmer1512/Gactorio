#include "model/Machine.hpp"
#include "model/ProductionTask.hpp"

#include <cassert>
#include <memory>

int main() {
    // VoltzClassic's first step is Mixing; pair it with a MixingStation
    // so the role-aware assignTask succeeds.
    const gactorio::VoltzClassic product;
    auto task = std::make_shared<gactorio::ProductionTask>(product);
    gactorio::MixingStation machine(1, "Mixer State Test");

    assert(machine.getStatus() == gactorio::MachineStatus::Idle);
    assert(machine.stateName() == "Idle");

    assert(machine.assignTask(task));
    assert(machine.getStatus() == gactorio::MachineStatus::Working);
    assert(machine.stateName() == "Working");

    machine.forceBreak();
    assert(machine.getStatus() == gactorio::MachineStatus::Broken);
    assert(machine.stateName() == "Broken");

    machine.update(100.0);
    assert(machine.getStatus() == gactorio::MachineStatus::Broken);

    machine.repair();
    assert(machine.getStatus() == gactorio::MachineStatus::Maintenance);
    assert(machine.stateName() == "Maintenance");

    machine.update(1.0);
    assert(machine.getStatus() == gactorio::MachineStatus::Maintenance);

    machine.update(1.0);
    assert(machine.getStatus() == gactorio::MachineStatus::Idle);
    assert(machine.stateName() == "Idle");

    return 0;
}
