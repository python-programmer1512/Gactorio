#include "model/events/EventBus.hpp"

namespace gactorio {

void EventBus::subscribe(Observer* observer) {
    if (observer != nullptr) {
        observers_.push_back(observer);
    }
}

void EventBus::publish(const Event& event) const {
    for (auto* observer : observers_) {
        observer->onEvent(event);
    }
}

} // namespace gactorio

