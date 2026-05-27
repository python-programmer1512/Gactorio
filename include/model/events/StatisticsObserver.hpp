#pragma once

#include "model/events/Observer.hpp"
#include "model/memento/FactoryMemento.hpp"

namespace gactorio {

class StatisticsObserver : public Observer {
public:
    void onEvent(const Event& event) override;

    int completedProductEvents() const;
    int startedTaskEvents() const;
    int completedStepEvents() const;
    int brokenMachineEvents() const;
    int repairedMachineEvents() const;
    int stateChangedEvents() const;
    StatisticsMemento exportState() const;
    void restoreState(const StatisticsMemento& state);

private:
    int completedProductEvents_ = 0;
    int startedTaskEvents_ = 0;
    int completedStepEvents_ = 0;
    int brokenMachineEvents_ = 0;
    int repairedMachineEvents_ = 0;
    int stateChangedEvents_ = 0;
};

using Statistics = StatisticsObserver;

} // namespace gactorio
