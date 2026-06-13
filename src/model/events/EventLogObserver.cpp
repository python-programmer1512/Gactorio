#include "model/events/EventLogObserver.hpp"

#include <utility>

// =============================================================================
// EventLogObserver.cpp — Event 값 객체 구현 + 이벤트 로그 누적 구현
// (Event 의 정의가 짧아 같은 .cpp 에 함께 둠.)
// =============================================================================

namespace gactorio {

// Event 생성자: 발생 시각/종류/발생 기계 ID/메시지를 불변으로 저장.
Event::Event(SimulationTime simulationTime, EventType type, MachineId sourceId, std::string message)
    : simulationTime_(simulationTime), type_(type), sourceId_(sourceId), message_(std::move(message)) {}

SimulationTime Event::simulationTime() const {
    return simulationTime_;
}

EventType Event::type() const {
    return type_;
}

MachineId Event::sourceId() const {
    return sourceId_;
}

const std::string& Event::message() const {
    return message_;
}

// 들어오는 이벤트를 그대로 누적(시간순). UI 이벤트 로그가 이 목록을 표시.
void EventLogObserver::onEvent(const Event& event) {
    events_.push_back(event);
}

const std::vector<Event>& EventLogObserver::events() const {
    return events_;
}

} // namespace gactorio

