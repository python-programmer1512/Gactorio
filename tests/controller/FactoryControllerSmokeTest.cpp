#include "controller/FactoryController.hpp"

#include <cassert>

int main() {
    gactorio::FactoryController controller;

    controller.pauseSimulation();
    controller.startSimulation();
    controller.setSpeed(2.0);

    assert(controller.enqueueProduct(1,   gactorio::ProductType::HyperBolt)
               == gactorio::FactoryCommandResult::Success);
    assert(controller.enqueueProduct(999, gactorio::ProductType::VoltzClassic)
               == gactorio::FactoryCommandResult::NotFound);
    assert(controller.enqueueProduct(1,   gactorio::ProductType::Unknown)
               == gactorio::FactoryCommandResult::InvalidRequest);

    auto snapshot = controller.getFactorySnapshot();
    assert(snapshot.productionLines().size() == 1);
    assert(snapshot.productionLines().front().queueLength() >= 1);

    assert(controller.forceBreak(1)    == gactorio::FactoryCommandResult::Success);
    assert(controller.repairMachine(1) == gactorio::FactoryCommandResult::Success);
    assert(controller.forceBreak(999)  == gactorio::FactoryCommandResult::NotFound);

    controller.tick(2.0);

    const auto logs  = controller.getEventLogs();
    const auto stats = controller.getStatistics();

    assert(!logs.empty());
    assert(stats.stateChangedEvents() > 0);

    controller.resetSimulation();
    snapshot = controller.getFactorySnapshot();
    assert(snapshot.simulationTime() == 0.0);

    return 0;
}
