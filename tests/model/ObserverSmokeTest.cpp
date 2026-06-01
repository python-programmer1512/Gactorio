#include "model/Machine.hpp"
#include "model/ProductionTask.hpp"
#include "model/events/EventBus.hpp"
#include "model/events/EventLogObserver.hpp"
#include "model/events/StatisticsObserver.hpp"

#include <cassert>
#include <memory>

int main() {
    gactorio::EventBus bus;
    gactorio::EventLogObserver log;
    gactorio::StatisticsObserver stats;

    bus.subscribe(&log);
    bus.subscribe(&stats);

    const gactorio::VoltzClassic product;
    auto task = std::make_shared<gactorio::ProductionTask>(product);
    gactorio::MixingStation machine(1, "Observed Mixer");
    machine.setEventBus(&bus);

    assert(machine.canAcceptTask());
    assert(task->currentStep() != nullptr);
    assert(machine.canProcess(task->currentStep()->requiredRole()));
    assert(machine.assignTask(task));

    // Advance long enough to finish the first 13-second mixing step.
    machine.update(13.5);
    machine.forceBreak();
    machine.repair();
    machine.update(4.0);  // wait out kRepairAllDelaySeconds (3 s)

    assert(!log.events().empty());
    assert(stats.startedTaskEvents() == 1);
    assert(stats.completedStepEvents() == 1);
    assert(stats.brokenMachineEvents() == 1);
    assert(stats.repairedMachineEvents() == 1);
    assert(stats.stateChangedEvents() >= 3);

    bool sawTaskEnqueued = false;
    bool sawStateChanged = false;
    for (const auto& event : log.events()) {
        if (event.type() == gactorio::EventType::TaskEnqueued) sawTaskEnqueued = true;
        if (event.type() == gactorio::EventType::StateChanged) sawStateChanged = true;
    }
    assert(sawTaskEnqueued);
    assert(sawStateChanged);

    return 0;
}
