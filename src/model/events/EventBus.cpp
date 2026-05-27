#include "model/events/EventBus.hpp"

#include <algorithm>

namespace gactorio {

void EventBus::subscribe(Observer* observer) {
    if (observer != nullptr && std::find(observers_.begin(), observers_.end(), observer) == observers_.end()) {
        observers_.push_back(observer);
    }
}

void EventBus::publish(const Event& event) const {
    for (auto* observer : observers_) {
        observer->onEvent(event);
    }
}

} // namespace gactorio
