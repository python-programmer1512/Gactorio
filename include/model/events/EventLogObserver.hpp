#pragma once

// =============================================================================
// EventLogObserver — 들어오는 모든 Event 를 시간순으로 저장하는 구독자
// -----------------------------------------------------------------------------
// Observer 를 구현. onEvent 가 불릴 때마다 events_ 벡터에 누적한다. UI 의 이벤트 로그
// 패널은 이 목록을 (스냅샷을 통해) 받아 화면에 뿌린다. 별칭 EventLog 로도 부른다.
// =============================================================================

#include "model/events/Event.hpp"
#include "model/events/Observer.hpp"

#include <vector>

namespace gactorio {

class EventLogObserver : public Observer {
public:
    void onEvent(const Event& event) override;       // 이벤트를 events_ 에 추가
    const std::vector<Event>& events() const;        // 누적된 이벤트 목록 읽기

private:
    std::vector<Event> events_;   // 이벤트 기록(Event 값 소유)
};

using EventLog = EventLogObserver;   // 가독성용 별칭

} // namespace gactorio
