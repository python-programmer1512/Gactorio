#include "model/Machine.hpp"
#include "model/ProductionTask.hpp"

#include <cassert>
#include <memory>

int main() {
    const gactorio::EnergyDrink product;
    auto task = std::make_shared<gactorio::ProductionTask>(product);
    gactorio::Carbonator machine(1, "Carbonator 1");

    assert(machine.getId() == 1);
    assert(machine.getName() == "Carbonator 1");
    assert(machine.getStatus() == gactorio::MachineStatus::Idle);
    assert(machine.getHealth() == 100.0);
    assert(machine.canAcceptTask());
    assert(machine.canProcess(gactorio::MachineRole::Carbonator));
    assert(!machine.canProcess(gactorio::MachineRole::Labeler));

    assert(machine.assignTask(task));
    assert(!machine.canAcceptTask());
    assert(machine.getStatus() == gactorio::MachineStatus::Working);

    machine.update(1.0);
    assert(machine.getProgress() > 0.08);
    assert(machine.getProgress() < 0.09);

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
