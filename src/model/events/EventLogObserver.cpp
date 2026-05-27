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

EventLogMemento EventLogObserver::exportState() const {
    EventLogMemento state;
    state.maxEvents = maxEvents_;
    state.events.reserve(events_.size());
    for (const auto& event : events_) {
        state.events.push_back(EventMemento{
            event.simulationTime(),
            event.type(),
            event.sourceId(),
            event.message()});
    }
    return state;
}

void EventLogObserver::restoreState(const EventLogMemento& state) {
    maxEvents_ = std::max<std::size_t>(1, state.maxEvents);
    events_.clear();
    events_.reserve(state.events.size());
    for (const auto& event : state.events) {
        events_.emplace_back(event.simulationTime, event.type, event.sourceId, event.message);
    }
    if (events_.size() > maxEvents_) {
        events_.erase(events_.begin(), events_.begin() + static_cast<std::vector<Event>::difference_type>(events_.size() - maxEvents_));
    }
}

} // namespace gactorio
