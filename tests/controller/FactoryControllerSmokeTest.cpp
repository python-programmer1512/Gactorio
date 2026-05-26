#include "controller/FactoryController.hpp"

#include <cassert>
#include <vector>

int main() {
    gactorio::FactoryController controller;

    controller.pauseSimulation();
    controller.startSimulation();
    controller.setSpeed(2.0);

<<<<<<< Updated upstream
    assert(controller.enqueueProduct(1, gactorio::ProductType::MetalBox) == gactorio::FactoryCommandResult::Success);
    assert(controller.enqueueProduct(999, gactorio::ProductType::ToyCar) == gactorio::FactoryCommandResult::NotFound);
    assert(controller.enqueueProduct(1, gactorio::ProductType::Unknown) == gactorio::FactoryCommandResult::InvalidRequest);
=======
    assert(controller.enqueueProduct(1,   gactorio::ProductType::HyperBolt)
               == gactorio::FactoryCommandResult::Success);
    assert(controller.enqueueProduct(999, gactorio::ProductType::VoltzClassic)
               == gactorio::FactoryCommandResult::NotFound);
    assert(controller.enqueueProduct(1,   gactorio::ProductType::Unknown)
               == gactorio::FactoryCommandResult::UnknownProduct);

    const auto logsAfterFirstSuccess = controller.getEventLogs();
    std::vector<gactorio::EventType> enqueueEvents;
    for (const auto& event : logsAfterFirstSuccess) {
        if (event.type() == gactorio::EventType::InputsConsumed ||
            event.type() == gactorio::EventType::TaskEnqueued) {
            enqueueEvents.push_back(event.type());
        }
    }
    assert(enqueueEvents.size() >= 2);
    assert(enqueueEvents[enqueueEvents.size() - 2] == gactorio::EventType::InputsConsumed);
    assert(enqueueEvents[enqueueEvents.size() - 1] == gactorio::EventType::TaskEnqueued);

    for (int i = 0; i < 47; ++i) {
        assert(controller.enqueueProduct(1, gactorio::ProductType::VoltzClassic)
                   == gactorio::FactoryCommandResult::Success);
    }
>>>>>>> Stashed changes

    auto snapshot = controller.getFactorySnapshot();
    const auto queueLengthBeforeShortage = snapshot.productionLines().front().queueLength();
    assert(controller.enqueueProduct(1, gactorio::ProductType::VoltzClassic)
               == gactorio::FactoryCommandResult::InsufficientMaterials);
    snapshot = controller.getFactorySnapshot();
    assert(snapshot.productionLines().front().queueLength() == queueLengthBeforeShortage);
    const auto logsAfterShortage = controller.getEventLogs();
    assert(!logsAfterShortage.empty());
    assert(logsAfterShortage.back().type() == gactorio::EventType::Info);

    assert(snapshot.productionLines().size() == 1);
    assert(snapshot.productionLines().front().queueLength() >= 1);

    assert(controller.forceBreak(1) == gactorio::FactoryCommandResult::Success);
    assert(controller.repairMachine(1) == gactorio::FactoryCommandResult::Success);
    assert(controller.forceBreak(999) == gactorio::FactoryCommandResult::NotFound);

    controller.tick(2.0);

    const auto logs = controller.getEventLogs();
    const auto stats = controller.getStatistics();

    assert(!logs.empty());
    assert(stats.stateChangedEvents() > 0);

    controller.resetSimulation();
    snapshot = controller.getFactorySnapshot();
    assert(snapshot.simulationTime() == 0.0);

    return 0;
}
