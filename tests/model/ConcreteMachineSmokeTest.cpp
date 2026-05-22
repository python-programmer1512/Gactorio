#include "model/Machine.hpp"
#include "model/ProductionTask.hpp"

#include <cassert>
#include <memory>
#include <vector>

int main() {
    gactorio::Cutter cutter(1, "Cutter 1", 1.5, 88.0, 0.07);
    gactorio::Conveyor conveyor(2, "Conveyor 1");
    gactorio::Assembler assembler(3, "Assembler 1");
    gactorio::Painter painter(4, "Painter 1");

    assert(cutter.canProcess(gactorio::MachineRole::Processor));
    assert(conveyor.canProcess(gactorio::MachineRole::Buffer));
    assert(assembler.canProcess(gactorio::MachineRole::Producer));
    assert(painter.canProcess(gactorio::MachineRole::Output));

    assert(cutter.getProcessingSpeed() == 1.5);
    assert(cutter.getHealth() == 88.0);
    assert(cutter.getBreakdownProbability() == 0.07);

    std::vector<std::unique_ptr<gactorio::Machine>> machines;
    machines.push_back(std::make_unique<gactorio::Cutter>(10, "Poly Cutter"));
    machines.push_back(std::make_unique<gactorio::Conveyor>(11, "Poly Conveyor"));
    machines.push_back(std::make_unique<gactorio::Assembler>(12, "Poly Assembler"));
    machines.push_back(std::make_unique<gactorio::Painter>(13, "Poly Painter"));

    for (auto& machine : machines) {
        machine->update(0.25);
        assert(machine->getHealth() > 0.0);
    }

    const gactorio::ToyCar toyCar;
    auto task = std::make_shared<gactorio::ProductionTask>(toyCar);

    assert(!painter.assignTask(task));
    assert(cutter.assignTask(task));

    return 0;
}
