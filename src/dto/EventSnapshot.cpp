#include "dto/EventSnapshot.hpp"

#include <utility>

namespace gactorio {

EventSnapshot::EventSnapshot(SimulationTime simulationTime, EventType type, std::string message)
    : simulationTime_(simulationTime), type_(type), message_(std::move(message)) {}

SimulationTime EventSnapshot::simulationTime() const {
    return simulationTime_;
}

SimulationTime EventSnapshot::timeSeconds() const {
    return simulationTime_;
}

EventType EventSnapshot::type() const {
    return type_;
}

const std::string& EventSnapshot::message() const {
    return message_;
}

} // namespace gactorio
