#include "model/Machine.hpp"
#include "model/ProductionTask.hpp"

#include <cassert>
#include <memory>

int main() {
    // VoltzClassic's first step is Mixing (13 seconds).
    const gactorio::VoltzClassic product;
    auto task = std::make_shared<gactorio::ProductionTask>(product);
    gactorio::MixingStation machine(1, "Mixer 1");

    assert(machine.getId() == 1);
    assert(machine.getName() == "Mixer 1");
    assert(machine.getStatus() == gactorio::MachineStatus::Idle);
    // Machine base class clamps health to [0, 100]; the JSON-spec 150 HP is
    // saturated to the cap on construction.
    assert(machine.getHealth() == 100.0);
    assert(machine.canAcceptTask());
    assert(machine.canProcess(gactorio::MachineRole::Mixing));
    assert(!machine.canProcess(gactorio::MachineRole::Packaging));

    assert(machine.assignTask(task));
    assert(!machine.canAcceptTask());
    assert(machine.getStatus() == gactorio::MachineStatus::Working);

    machine.update(1.0);
    assert(machine.getProgress() > 0.0);
    assert(machine.getProgress() < 1.0);

    const auto snapshot = machine.getSnapshot();
    assert(snapshot.id() == machine.getId());
    assert(snapshot.status() == gactorio::MachineStatus::Working);
    assert(snapshot.health() == 100.0);

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
