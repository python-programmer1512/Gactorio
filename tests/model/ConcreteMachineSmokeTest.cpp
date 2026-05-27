#include "model/Machine.hpp"
#include "model/ProductionTask.hpp"

#include <cassert>
#include <memory>
#include <vector>

int main() {
    gactorio::Carbonator carbonator(1, "Carbonator 1", 1.5, 88.0, 0.07);
    gactorio::Conveyor conveyor(2, "Conveyor 1");
    gactorio::Filler filler(3, "Filler 1");
    gactorio::Sealer sealer(4, "Sealer 1");
    gactorio::Labeler labeler(5, "Labeler 1");

    assert(carbonator.canProcess(gactorio::MachineRole::Carbonator));
    assert(conveyor.canProcess(gactorio::MachineRole::Conveyor));
    assert(filler.canProcess(gactorio::MachineRole::Filler));
    assert(sealer.canProcess(gactorio::MachineRole::Sealer));
    assert(labeler.canProcess(gactorio::MachineRole::Labeler));

    assert(carbonator.getProcessingSpeed() == 1.5);
    assert(carbonator.getHealth() == 88.0);
    assert(carbonator.getBreakdownProbability() == 0.07);

    std::vector<std::unique_ptr<gactorio::Machine>> machines;
    machines.push_back(std::make_unique<gactorio::Carbonator>(10, "Poly Carbonator"));
    machines.push_back(std::make_unique<gactorio::Conveyor>(11, "Poly Conveyor"));
    machines.push_back(std::make_unique<gactorio::Filler>(12, "Poly Filler"));
    machines.push_back(std::make_unique<gactorio::Sealer>(13, "Poly Sealer"));
    machines.push_back(std::make_unique<gactorio::Labeler>(14, "Poly Labeler"));

    for (auto& machine : machines) {
        machine->update(0.25);
        assert(machine->getHealth() > 0.0);
    }

    const gactorio::SodaCan sodaCan;
    auto task = std::make_shared<gactorio::ProductionTask>(sodaCan);

    assert(!labeler.assignTask(task));
    assert(carbonator.assignTask(task));

    return 0;
}
