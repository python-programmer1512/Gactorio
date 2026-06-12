#include "model/ProductionLine.hpp"

#include <cassert>
#include <memory>

int main() {
    gactorio::ProductionLine line(1, "Beverage Line A");
    line.addMachine(std::make_unique<gactorio::MixingStation>   (1, "Mixer",    1.0, 150.0, 0.0));
    line.addMachine(std::make_unique<gactorio::QualityStation>  (2, "Quality",  1.0, 100.0, 0.0));
    line.addMachine(std::make_unique<gactorio::BottlingStation> (3, "Bottling", 1.0, 120.0, 0.0));
    line.addMachine(std::make_unique<gactorio::PackagingStation>(4, "Packaging",1.0, 140.0, 0.0));

    line.enqueueProduct(std::make_shared<gactorio::VoltzClassic>());
    line.enqueueProduct(std::make_shared<gactorio::HyperBolt>());

    assert(line.queueLength() == 2);
    assert(line.currentTask() != nullptr);
    assert(line.currentTask()->getProductName() == "Voltz Classic");

    assert(line.queueLength() == 2);
    assert(line.currentTask()->getProductName() == "Voltz Classic");
    assert(line.machines().size() == 4);

    // First update routes the task to the Mixing station.
    line.update(0.0);
    assert(line.machines().front()->getStatus() == gactorio::MachineStatus::Working);
    assert(line.machines().back()->getStatus()  == gactorio::MachineStatus::Idle);

    // When the first drink leaves Mixing, Mixing immediately starts the next
    // queued drink while Quality starts the first one.
    line.update(13.0);
    assert(line.machines()[0]->getStatus() == gactorio::MachineStatus::Working);
    assert(line.machines()[1]->getStatus() == gactorio::MachineStatus::Working);

    // Hyper Bolt's Mixing stage comes from data/factory_config.json: 18 sec.
    line.update(17.0);
    assert(line.machines()[0]->getStatus() == gactorio::MachineStatus::Working);
    line.update(1.0);
    assert(line.machines()[0]->getStatus() == gactorio::MachineStatus::Idle ||
           line.machines()[0]->getStatus() == gactorio::MachineStatus::Working);

    // Drive the remaining pipelined work to completion.
    for (int i = 0; i < 30 && line.queueLength() > 0; ++i) {
        line.update(5.0);
    }
    assert(line.queueLength() == 0);

    assert(line.queueLength() == 0);
    assert(line.currentTask() == nullptr);

    return 0;
}
