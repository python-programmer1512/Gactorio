#pragma once

// =============================================================================
// StatisticsSnapshot — 집계 통계의 읽기 전용 DTO
// -----------------------------------------------------------------------------
// StatisticsObserver 의 카운터들을 한 번에 복사해 담는다. 첫 인자만 필수이고
// 나머지는 기본값 0 — 공장이 없을 때 빈 통계를 만들기 쉽게 하기 위함.
// =============================================================================

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

    int completedProductEvents() const;   // 완료 제품 수
    int startedTaskEvents() const;        // 시작 작업 수
    int completedStepEvents() const;      // 완료 공정 단계 수
    int brokenMachineEvents() const;      // 고장 횟수
    int repairedMachineEvents() const;    // 수리 횟수
    int stateChangedEvents() const;       // 상태 전이 횟수

private:
    int completedProductEvents_;
    int startedTaskEvents_;
    int completedStepEvents_;
    int brokenMachineEvents_;
    int repairedMachineEvents_;
    int stateChangedEvents_;
};

} // namespace gactorio
