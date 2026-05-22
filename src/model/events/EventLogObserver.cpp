#include "model/events/EventLogObserver.hpp"

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

void EventLogObserver::onEvent(const Event& event) {
    events_.push_back(event);
}

const std::vector<Event>& EventLogObserver::events() const {
    return events_;
}

} // namespace gactorio

