#include "model/events/StatisticsObserver.hpp"

#include "model/events/Event.hpp"

// =============================================================================
// StatisticsObserver.cpp — 이벤트 종류별 카운터 집계 구현
// =============================================================================

namespace gactorio {

// 이벤트 종류로 분기해 해당 카운터를 1 증가. 관심 없는 종류는 default 로 무시.
void StatisticsObserver::onEvent(const Event& event) {
    switch (event.type()) {
    case EventType::TaskStarted:
        ++startedTaskEvents_;
        break;
    case EventType::StepCompleted:
        ++completedStepEvents_;
        break;
    case EventType::ProductCompleted:
        ++completedProductEvents_;
        break;
    case EventType::MachineBroken:
        ++brokenMachineEvents_;
        break;
    case EventType::MachineRepaired:
        ++repairedMachineEvents_;
        break;
    case EventType::StateChanged:
        ++stateChangedEvents_;
        break;
    default:
        break;
    }
}

int StatisticsObserver::completedProductEvents() const {
    return completedProductEvents_;
}

int StatisticsObserver::startedTaskEvents() const {
    return startedTaskEvents_;
}

int StatisticsObserver::completedStepEvents() const {
    return completedStepEvents_;
}

int StatisticsObserver::brokenMachineEvents() const {
    return brokenMachineEvents_;
}

int StatisticsObserver::repairedMachineEvents() const {
    return repairedMachineEvents_;
}

int StatisticsObserver::stateChangedEvents() const {
    return stateChangedEvents_;
}

} // namespace gactorio
