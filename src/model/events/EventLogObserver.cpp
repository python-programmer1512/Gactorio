#include "model/events/EventLogObserver.hpp"

#include <algorithm>
#include <utility>

namespace gactorio {

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

EventLogObserver::EventLogObserver(std::size_t maxEvents)
    : maxEvents_(std::max<std::size_t>(1, maxEvents)) {}

void EventLogObserver::onEvent(const Event& event) {
    events_.push_back(event);
    if (events_.size() > maxEvents_) {
        events_.erase(events_.begin());
    }
}

const std::vector<Event>& EventLogObserver::events() const {
    return events_;
}

} // namespace gactorio
