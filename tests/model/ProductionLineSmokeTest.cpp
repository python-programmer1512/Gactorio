#include "model/ProductionLine.hpp"

#include <cassert>
#include <memory>

int main() {
    gactorio::ProductionLine line(1, "Beverage Line A");
    line.addMachine(std::make_unique<gactorio::MixingStation>   (1, "Mixer"));
    line.addMachine(std::make_unique<gactorio::QualityStation>  (2, "Quality"));
    line.addMachine(std::make_unique<gactorio::BottlingStation> (3, "Bottling"));
    line.addMachine(std::make_unique<gactorio::PackagingStation>(4, "Packaging"));

    line.enqueueProduct(std::make_shared<gactorio::VoltzClassic>());

    assert(line.queueLength() == 1);
    assert(line.currentTask() != nullptr);
    assert(line.currentTask()->getProductName() == "Voltz Classic");

    auto snapshot = line.getSnapshot();
    assert(snapshot.queueLength() == 1);
    assert(snapshot.currentTaskName() == "Voltz Classic");
    assert(snapshot.machines().size() == 4);

    // First update routes the task to the Mixing station.
    line.update(0.0);
    assert(line.machines().front()->getStatus() == gactorio::MachineStatus::Working);
    assert(line.machines().back()->getStatus()  == gactorio::MachineStatus::Idle);

    // Drive the whole 40-second route through the four stations.
    for (int i = 0; i < 30 && line.queueLength() > 0; ++i) {
        line.update(5.0);
    }
    assert(line.queueLength() == 0);

    snapshot = line.getSnapshot();
    assert(snapshot.queueLength() == 0);
    assert(snapshot.currentTaskName().empty());

    return 0;
}
