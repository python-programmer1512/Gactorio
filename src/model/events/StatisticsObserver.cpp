#include "model/events/StatisticsObserver.hpp"

#include "model/events/Event.hpp"

namespace gactorio {

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

StatisticsMemento StatisticsObserver::exportState() const {
    StatisticsMemento state;
    state.completedProductEvents = completedProductEvents_;
    state.startedTaskEvents = startedTaskEvents_;
    state.completedStepEvents = completedStepEvents_;
    state.brokenMachineEvents = brokenMachineEvents_;
    state.repairedMachineEvents = repairedMachineEvents_;
    state.stateChangedEvents = stateChangedEvents_;
    return state;
}

void StatisticsObserver::restoreState(const StatisticsMemento& state) {
    completedProductEvents_ = state.completedProductEvents;
    startedTaskEvents_ = state.startedTaskEvents;
    completedStepEvents_ = state.completedStepEvents;
    brokenMachineEvents_ = state.brokenMachineEvents;
    repairedMachineEvents_ = state.repairedMachineEvents;
    stateChangedEvents_ = state.stateChangedEvents;
}

} // namespace gactorio
