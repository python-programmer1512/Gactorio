#pragma once

// =============================================================================
// Event — "시뮬레이션에서 무슨 일이 일어났는가"를 나타내는 불변 값 객체
// -----------------------------------------------------------------------------
// Observer 패턴에서 발행자(Machine/ProductionLine)가 만들어 EventBus 로 보내는 메시지.
// 발생 시각, 종류(EventType), 발생 기계 ID, 사람이 읽는 메시지를 담는다.
// 생성 후에는 바뀌지 않도록 모든 필드 private + 읽기 전용 접근자만 제공.
// =============================================================================

#include "common/Types.hpp"

#include <string>

namespace gactorio {

struct Event {
public:
    Event(SimulationTime simulationTime, EventType type, MachineId sourceId, std::string message);

    SimulationTime simulationTime() const;   // 발생 시각(시뮬레이션 초)
    EventType type() const;                  // 이벤트 종류
    MachineId sourceId() const;              // 발생시킨 기계 ID(라인 이벤트는 0)
    const std::string& message() const;      // 사람이 읽는 설명

private:
    SimulationTime simulationTime_;
    EventType type_;
    MachineId sourceId_;
    std::string message_;
};

} // namespace gactorio
