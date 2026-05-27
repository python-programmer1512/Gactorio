#pragma once

#include "model/events/Event.hpp"
#include "model/events/Observer.hpp"
#include "model/memento/FactoryMemento.hpp"

#include <cstddef>
#include <vector>

namespace gactorio {

class EventLogObserver : public Observer {
public:
    explicit EventLogObserver(std::size_t maxEvents = 500);

    void onEvent(const Event& event) override;
    const std::vector<Event>& events() const;
    EventLogMemento exportState() const;
    void restoreState(const EventLogMemento& state);

private:
    std::size_t maxEvents_;
    std::vector<Event> events_;
};

using EventLog = EventLogObserver;

} // namespace gactorio
