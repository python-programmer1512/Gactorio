#include "common/SimClock.hpp"

#include <algorithm>

// =============================================================================
// SimClock.cpp — 시계 구현. pause/stop 이면 시간 진행을 멈춰(0 반환) "일시정지 중
// tick 호출은 무해한 no-op" 이 되게 한다(JS 가 매 프레임 tick 해도 안전).
// =============================================================================

namespace gactorio {

// 실시간 delta → (배속 적용) 시뮬레이션 delta. 일시정지/정지면 0 반환(시간 안 흐름).
SimulationTime SimClock::update(double realDeltaTime) {
    if (paused_ || stopped_) {
        lastDeltaTime_ = 0.0;
        return lastDeltaTime_;
    }

    lastDeltaTime_ = std::max(0.0, realDeltaTime) * speedMultiplier_;  // 음수 delta 방지 후 배속 적용
    currentTime_ += lastDeltaTime_;
    return lastDeltaTime_;
}

void SimClock::pause() {              // 일시정지
    paused_ = true;
}

void SimClock::resume() {             // 재개(정지/일시정지 모두 해제)
    paused_ = false;
    stopped_ = false;
}

void SimClock::stop() {               // 정지(시간값은 유지, delta 0)
    stopped_ = true;
    paused_ = false;
    lastDeltaTime_ = 0.0;
}

void SimClock::reset() {              // 완전 초기화
    currentTime_ = 0.0;
    lastDeltaTime_ = 0.0;
    speedMultiplier_ = 1.0;
    paused_ = false;
    stopped_ = false;
}

void SimClock::setSpeed(double speedMultiplier) {   // 배속 설정(음수는 0으로)
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

// Memento 복원용: 시간을 저장값으로 즉시 점프(음수 방지). delta 는 0으로 리셋.
void SimClock::setNow(SimulationTime t) {
    currentTime_   = std::max(0.0, t);
    lastDeltaTime_ = 0.0;
}

} // namespace gactorio

