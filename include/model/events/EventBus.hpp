#pragma once

// =============================================================================
// EventBus — the Subject of the Observer pattern. subscribe() registers an
// Observer*; publish() fans an Event out to every subscriber. The vector holds
// non-owning Observer* (association ◇) — the Factory owns the observers.
// See Observer_Event_Pattern_Diagram.
// =============================================================================

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

