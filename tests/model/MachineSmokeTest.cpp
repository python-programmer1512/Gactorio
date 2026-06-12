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
    assert(machine.getHealth() == 100.0);
    assert(machine.canAcceptTask());
    assert(machine.canProcess(gactorio::MachineRole::Mixing));
    assert(!machine.canProcess(gactorio::MachineRole::Packaging));

    assert(machine.assignTask(task));
    assert(!machine.canAcceptTask());
    assert(machine.getStatus() == gactorio::MachineStatus::Working);

    // After one update, the machine is either still working (most likely) or
    // already broken if random wear-and-tear depleted HP to zero. Both are
    // valid outcomes — just sanity-check the bounds.
    machine.update(1.0);
    assert(machine.getProgress() >= 0.0);
    assert(machine.getProgress() <= 1.0);

    assert(machine.getId() == 1);
    assert(machine.getStatus() == gactorio::MachineStatus::Working ||
           machine.getStatus() == gactorio::MachineStatus::Broken);
    assert(machine.getHealth() >= 0.0 && machine.getHealth() <= 100.0);

    machine.forceBreak();
    assert(machine.getStatus() == gactorio::MachineStatus::Broken);
    assert(machine.getHealth() == 0.0);
    assert(!machine.canAcceptTask());

    machine.repair();
    assert(machine.getStatus() == gactorio::MachineStatus::Maintenance);
    assert(machine.getHealth() == 0.0);
    assert(!machine.canAcceptTask());

    // forceBreak cleared the task, so after maintenance completes the
    // machine returns to Idle (not Working). Maintenance duration is now
    // config::kRepairAllDelaySeconds (3 s), so we feed a 4 s tick.
    machine.update(4.0);
    assert(machine.getStatus() == gactorio::MachineStatus::Idle);
    assert(machine.getHealth() == 100.0);
    assert(machine.canAcceptTask());

    return 0;
}
