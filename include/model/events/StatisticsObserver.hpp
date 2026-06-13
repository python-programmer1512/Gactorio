#pragma once

// =============================================================================
// StatisticsObserver — Event 종류별로 카운트를 집계하는 구독자
// -----------------------------------------------------------------------------
// Observer 를 구현. onEvent 안에서 EventType 으로 분기해 해당 카운터를 증가시킨다.
// "UI가 기계들을 일일이 스캔해 통계를 재계산"하지 않고, 이벤트가 발생할 때 누적하는
// 구조 — 통계 책임을 한곳에 캡슐화(SRP). 별칭 Statistics 로도 부른다.
// =============================================================================

#include "model/events/Observer.hpp"

namespace gactorio {

class StatisticsObserver : public Observer {
public:
    void onEvent(const Event& event) override;   // 종류별 카운터 증가

    int completedProductEvents() const;   // 완료된 제품 수
    int startedTaskEvents() const;        // 시작된 작업 수
    int completedStepEvents() const;      // 완료된 공정 단계 수
    int brokenMachineEvents() const;      // 고장 발생 횟수
    int repairedMachineEvents() const;    // 수리 완료 횟수
    int stateChangedEvents() const;       // 상태 전이 횟수

private:
    int completedProductEvents_ = 0;
    int startedTaskEvents_ = 0;
    int completedStepEvents_ = 0;
    int brokenMachineEvents_ = 0;
    int repairedMachineEvents_ = 0;
    int stateChangedEvents_ = 0;
};

using Statistics = StatisticsObserver;   // 가독성용 별칭

} // namespace gactorio
