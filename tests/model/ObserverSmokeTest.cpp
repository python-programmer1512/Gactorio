#include "model/Machine.hpp"
#include "model/ProductionTask.hpp"
#include "model/events/EventBus.hpp"
#include "model/events/EventLogObserver.hpp"
#include "model/events/StatisticsObserver.hpp"

#include <cassert>
#include <memory>

int main() {
    gactorio::EventBus bus;
    gactorio::EventLogObserver log(64);
    gactorio::StatisticsObserver stats;

    bus.subscribe(&log);
    bus.subscribe(&log);
    bus.subscribe(&stats);

    const gactorio::SodaCan product;
    auto task = std::make_shared<gactorio::ProductionTask>(product);
    gactorio::Carbonator machine(1, "Observed Carbonator");
    machine.setEventBus(&bus);

    assert(machine.canAcceptTask());
    assert(task->currentStep() != nullptr);
    assert(machine.canProcess(task->currentStep()->requiredRole()));
    assert(machine.assignTask(task));
    machine.update(4.0);
    machine.forceBreak();
    machine.repair();
    machine.update(2.0);

    assert(!log.events().empty());
    assert(stats.startedTaskEvents() == 1);
    assert(stats.completedStepEvents() == 1);
    assert(stats.brokenMachineEvents() == 1);
    assert(stats.repairedMachineEvents() == 1);
    assert(stats.stateChangedEvents() >= 3);

    const auto savedStats = stats.exportState();
    stats.onEvent(gactorio::Event(3.0, gactorio::EventType::ProductCompleted, 1, "extra completed product"));
    stats.onEvent(gactorio::Event(4.0, gactorio::EventType::MachineBroken, 1, "extra breakdown"));
    assert(stats.completedProductEvents() == savedStats.completedProductEvents + 1);
    assert(stats.brokenMachineEvents() == savedStats.brokenMachineEvents + 1);

    stats.restoreState(savedStats);
    assert(stats.completedProductEvents() == savedStats.completedProductEvents);
    assert(stats.startedTaskEvents() == savedStats.startedTaskEvents);
    assert(stats.completedStepEvents() == savedStats.completedStepEvents);
    assert(stats.brokenMachineEvents() == savedStats.brokenMachineEvents);
    assert(stats.repairedMachineEvents() == savedStats.repairedMachineEvents);
    assert(stats.stateChangedEvents() == savedStats.stateChangedEvents);

    bool sawTaskEnqueued = false;
    bool sawStateChanged = false;
    for (const auto& event : log.events()) {
        if (event.type() == gactorio::EventType::TaskEnqueued) {
            sawTaskEnqueued = true;
        }
        if (event.type() == gactorio::EventType::StateChanged) {
            sawStateChanged = true;
        }
    }

    assert(sawTaskEnqueued);
    assert(sawStateChanged);

    gactorio::EventLogObserver limitedLog(2);
    limitedLog.onEvent(gactorio::Event(0.0, gactorio::EventType::Info, 1, "first beverage event"));
    limitedLog.onEvent(gactorio::Event(1.0, gactorio::EventType::Info, 1, "second beverage event"));
    limitedLog.onEvent(gactorio::Event(2.0, gactorio::EventType::Info, 1, "third beverage event"));
    assert(limitedLog.events().size() == 2);
    assert(limitedLog.events().front().message() == std::string("second beverage event"));

    const auto savedLog = limitedLog.exportState();
    assert(savedLog.maxEvents == 2);
    assert(savedLog.events.size() == 2);
    assert(savedLog.events.front().message == std::string("second beverage event"));

    limitedLog.onEvent(gactorio::Event(3.0, gactorio::EventType::MachineBroken, 2, "fourth beverage event"));
    assert(limitedLog.events().size() == 2);
    assert(limitedLog.events().front().message() == std::string("third beverage event"));

    limitedLog.restoreState(savedLog);
    assert(limitedLog.events().size() == 2);
    assert(limitedLog.events().front().simulationTime() == 1.0);
    assert(limitedLog.events().front().type() == gactorio::EventType::Info);
    assert(limitedLog.events().front().sourceId() == 1);
    assert(limitedLog.events().front().message() == std::string("second beverage event"));
    assert(limitedLog.events().back().simulationTime() == 2.0);
    assert(limitedLog.events().back().message() == std::string("third beverage event"));

    limitedLog.onEvent(gactorio::Event(4.0, gactorio::EventType::Info, 3, "after restore"));
    assert(limitedLog.events().size() == 2);
    assert(limitedLog.events().front().message() == std::string("third beverage event"));
    assert(limitedLog.events().back().message() == std::string("after restore"));

    return 0;
}
