#pragma once

#include "common/Types.hpp"

namespace gactorio {

class SimClock {
public:
    SimulationTime update(double realDeltaTime);

    void pause();
    void resume();
    void stop();
    void reset();
    void setSpeed(double speedMultiplier);
    // For Memento restore: jump clock straight to a saved value.
    void setNow(SimulationTime t);

    SimulationTime now() const;
    SimulationTime getDeltaTime() const;
    double speedMultiplier() const;
    bool isPaused() const;
    bool isStopped() const;

private:
    SimulationTime currentTime_ = 0.0;
    SimulationTime lastDeltaTime_ = 0.0;
    double speedMultiplier_ = 1.0;
    bool paused_ = false;
    bool stopped_ = false;
};

} // namespace gactorio

