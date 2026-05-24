#include "model/ProductionLine.hpp"

#include <cassert>
#include <memory>

int main() {
    gactorio::ProductionLine line(1, "Routing Line");
    line.addMachine(std::make_unique<gactorio::Cutter>(1, "Cutter"));
    line.addMachine(std::make_unique<gactorio::Conveyor>(2, "Conveyor C1"));
    line.addMachine(std::make_unique<gactorio::Assembler>(3, "Assembler"));
    line.addMachine(std::make_unique<gactorio::Conveyor>(4, "Conveyor C2"));
    line.addMachine(std::make_unique<gactorio::Painter>(5, "Painter"));

    line.enqueueProduct(std::make_shared<gactorio::ToyCar>());

    assert(line.queueLength() == 1);
    assert(line.currentTask() != nullptr);
    assert(line.currentTask()->getProductName() == "Toy Car");

    auto snapshot = line.getSnapshot();
    assert(snapshot.queueLength() == 1);
    assert(snapshot.currentTaskName() == "Toy Car");
    assert(snapshot.machines().size() == 5);

    line.update(0.0);
    assert(line.machines().front()->getStatus() == gactorio::MachineStatus::Working);
    assert(line.machines().back()->getStatus() == gactorio::MachineStatus::Idle);

    line.update(2.0);
    assert(line.currentTask() != nullptr);
    assert(line.currentTask()->currentStepIndex() == 1);
    assert(line.machines().front()->getStatus() == gactorio::MachineStatus::Idle);

    line.update(0.0);
    assert(line.machines()[1]->getStatus() == gactorio::MachineStatus::Working);

    for (int i = 0; i < 10 && line.queueLength() > 0; ++i) {
        line.update(10.0);
    }
    assert(line.queueLength() == 0);

    snapshot = line.getSnapshot();
    assert(snapshot.queueLength() == 0);
    assert(snapshot.currentTaskName().empty());

    return 0;
}
