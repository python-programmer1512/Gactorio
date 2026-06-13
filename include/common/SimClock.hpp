#pragma once

// =============================================================================
// SimClock — 시뮬레이션 "시계"
// -----------------------------------------------------------------------------
// 역할: 실제 경과 시간(real delta time)을 받아 배속(speedMultiplier)을 곱한
//       "시뮬레이션 delta time"으로 변환하고, 누적 시뮬레이션 시간을 관리한다.
//       또한 pause/stop/reset 같은 재생 제어 상태를 보관한다.
//
// 설계 의도(SRP): "시간"이라는 단 하나의 책임만 가진다. 기계/재고/이벤트는 모른다.
// Factory 가 이 시계를 값 멤버로 소유(composition)한다.
// =============================================================================

#include "common/Types.hpp"

namespace gactorio {

class SimClock {
public:
    // 실시간 delta 를 받아 (배속 적용된) 시뮬레이션 delta 를 반환하고 현재시간에 누적.
    // pause/stop 상태면 0 을 반환(시간 진행 없음).
    SimulationTime update(double realDeltaTime);

    void pause();    // 일시정지: update()가 0 반환
    void resume();   // 재개: pause/stop 해제
    void stop();     // 정지: 시간 진행 중단(시간값은 유지)
    void reset();    // 초기화: 시간 0, 배속 1.0, 상태 해제
    void setSpeed(double speedMultiplier);  // 배속 설정(음수는 0으로 클램프)
    // Memento 복원용: 시계를 저장된 시간값으로 즉시 점프.
    void setNow(SimulationTime t);

    SimulationTime now() const;            // 현재 누적 시뮬레이션 시간
    SimulationTime getDeltaTime() const;   // 직전 update 가 만든 delta
    double speedMultiplier() const;        // 현재 배속
    bool isPaused() const;
    bool isStopped() const;

private:
    SimulationTime currentTime_ = 0.0;     // 누적 시뮬레이션 시간(초)
    SimulationTime lastDeltaTime_ = 0.0;   // 직전 프레임의 시뮬레이션 delta
    double speedMultiplier_ = 1.0;         // 배속(1.0 = 실시간)
    bool paused_ = false;                  // 일시정지 플래그
    bool stopped_ = false;                 // 정지 플래그
};

} // namespace gactorio
