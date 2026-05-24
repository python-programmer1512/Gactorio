#pragma once

namespace gactorio {

class StatisticsSnapshot {
public:
    StatisticsSnapshot(
        int completedProductEvents,
        int startedTaskEvents = 0,
        int completedStepEvents = 0,
        int brokenMachineEvents = 0,
        int repairedMachineEvents = 0,
        int stateChangedEvents = 0);

    int completedProductEvents() const;
    int startedTaskEvents() const;
    int completedStepEvents() const;
    int brokenMachineEvents() const;
    int repairedMachineEvents() const;
    int stateChangedEvents() const;

private:
    int completedProductEvents_;
    int startedTaskEvents_;
    int completedStepEvents_;
    int brokenMachineEvents_;
    int repairedMachineEvents_;
    int stateChangedEvents_;
};

} // namespace gactorio
