#pragma once

// =============================================================================
// EventBus — Observer 패턴의 "주제(Subject)/발행 채널"
// -----------------------------------------------------------------------------
// 구독자(Observer*) 목록을 들고 있다가, publish(event)가 호출되면 모든 구독자에게
// onEvent 를 전달한다. 구독자는 외부(Factory)가 소유하므로 여기서는 비소유 raw
// 포인터로만 보관한다(observers_). 그래서 subscribe 만 있고 unsubscribe 는 없다 —
// 구독자 수명은 Factory 가 EventBus 보다 길게 보장한다.
// =============================================================================

#include "model/events/Event.hpp"
#include "model/events/Observer.hpp"

#include <vector>

namespace gactorio {

class EventBus {
public:
    void subscribe(Observer* observer);          // 구독자 등록(nullptr 무시)
    void publish(const Event& event) const;      // 모든 구독자에게 이벤트 전달

private:
    std::vector<Observer*> observers_;   // 비소유 구독자 포인터 목록
};

} // namespace gactorio
