#include "common/SimClock.hpp"

#include <algorithm>

namespace gactorio {

SimulationTime SimClock::update(double realDeltaTime) {
    if (paused_ || stopped_) {
        lastDeltaTime_ = 0.0;
        return lastDeltaTime_;
    }

    lastDeltaTime_ = std::max(0.0, realDeltaTime) * speedMultiplier_;
    currentTime_ += lastDeltaTime_;
    return lastDeltaTime_;
}

void SimClock::pause() {
    paused_ = true;
}

void SimClock::resume() {
    paused_ = false;
    stopped_ = false;
}

void SimClock::stop() {
    stopped_ = true;
    paused_ = false;
    lastDeltaTime_ = 0.0;
}

void SimClock::reset() {
    currentTime_ = 0.0;
    lastDeltaTime_ = 0.0;
    speedMultiplier_ = 1.0;
    paused_ = false;
    stopped_ = false;
}

void SimClock::setSpeed(double speedMultiplier) {
    speedMultiplier_ = std::max(0.0, speedMultiplier);
}

SimulationTime SimClock::now() const {
    return currentTime_;
}

SimulationTime SimClock::getDeltaTime() const {
    return lastDeltaTime_;
}

double SimClock::speedMultiplier() const {
    return speedMultiplier_;
}

bool SimClock::isPaused() const {
    return paused_;
}

bool SimClock::isStopped() const {
    return stopped_;
}

void SimClock::setNow(SimulationTime t) {
    currentTime_   = std::max(0.0, t);
    lastDeltaTime_ = 0.0;
}

} // namespace gactorio

