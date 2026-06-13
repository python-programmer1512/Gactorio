#include "model/events/EventBus.hpp"

// =============================================================================
// EventBus.cpp — Observer 패턴의 발행 구현. 구독자는 비소유 포인터로만 보관한다.
// =============================================================================

namespace gactorio {

// 구독자 등록(nullptr 은 무시). 소유하지 않으므로 수명은 외부(Factory)가 책임.
void EventBus::subscribe(Observer* observer) {
    if (observer != nullptr) {
        observers_.push_back(observer);
    }
}

// 이벤트를 모든 구독자에게 순서대로 전달. 발행자는 누가 어떻게 처리하는지 모른다.
void EventBus::publish(const Event& event) const {
    for (auto* observer : observers_) {
        observer->onEvent(event);
    }
}

} // namespace gactorio

