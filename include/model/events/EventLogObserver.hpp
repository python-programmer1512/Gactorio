#pragma once

#include "model/events/Event.hpp"
#include "model/events/Observer.hpp"

#include <vector>

namespace gactorio {

class EventLogObserver : public Observer {
public:
    void onEvent(const Event& event) override;
    const std::vector<Event>& events() const;

private:
    std::vector<Event> events_;
};

using EventLog = EventLogObserver;

} // namespace gactorio

