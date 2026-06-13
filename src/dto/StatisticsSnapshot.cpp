#include "dto/StatisticsSnapshot.hpp"

// =============================================================================
// StatisticsSnapshot.cpp — 통계 DTO 구현(6개 카운터의 값 복사 + 접근자).
// =============================================================================

namespace gactorio {

StatisticsSnapshot::StatisticsSnapshot(
    int completedProductEvents,
    int startedTaskEvents,
    int completedStepEvents,
    int brokenMachineEvents,
    int repairedMachineEvents,
    int stateChangedEvents)
    : completedProductEvents_(completedProductEvents),
      startedTaskEvents_(startedTaskEvents),
      completedStepEvents_(completedStepEvents),
      brokenMachineEvents_(brokenMachineEvents),
      repairedMachineEvents_(repairedMachineEvents),
      stateChangedEvents_(stateChangedEvents) {}

int StatisticsSnapshot::completedProductEvents() const {
    return completedProductEvents_;
}

int StatisticsSnapshot::startedTaskEvents() const {
    return startedTaskEvents_;
}

int StatisticsSnapshot::completedStepEvents() const {
    return completedStepEvents_;
}

int StatisticsSnapshot::brokenMachineEvents() const {
    return brokenMachineEvents_;
}

int StatisticsSnapshot::repairedMachineEvents() const {
    return repairedMachineEvents_;
}

int StatisticsSnapshot::stateChangedEvents() const {
    return stateChangedEvents_;
}

} // namespace gactorio