#pragma once

#include "model/events/Event.hpp"
#include "model/events/Observer.hpp"

#include <vector>

namespace gactorio {

class EventBus {
public:
    void subscribe(Observer* observer);
    void publish(const Event& event) const;

private:
    std::vector<Observer*> observers_;
};

} // namespace gactorio

