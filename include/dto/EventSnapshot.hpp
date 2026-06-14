#pragma once

// =============================================================================
// EventSnapshot — 이벤트 로그 항목 한 줄의 읽기 전용 DTO
// -----------------------------------------------------------------------------
// Model 의 Event 에서 표시에 필요한 (시각, 종류, 메시지)만 복사해 담는다.
// (Event 의 sourceId 같은 내부 식별자는 표시에 불필요하여 제외.)
// =============================================================================

#include "common/Types.hpp"

#include <string>

namespace gactorio {

class EventSnapshot {
public:
    EventSnapshot(SimulationTime simulationTime, EventType type, std::string message);

    SimulationTime simulationTime() const;   // 발생 시각
    SimulationTime timeSeconds() const;      // (동의어) 발생 시각(초)
    EventType type() const;                  // 이벤트 종류
    const std::string& message() const;      // 표시 메시지

private:
    SimulationTime simulationTime_;
    EventType type_;
    std::string message_;
};

} // namespace gactorio
