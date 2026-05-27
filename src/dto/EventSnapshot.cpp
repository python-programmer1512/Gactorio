#include "dto/EventSnapshot.hpp"

#include <utility>

namespace gactorio {

EventSnapshot::EventSnapshot(SimulationTime simulationTime, EventType type, MachineId sourceId, std::string message)
    : simulationTime_(simulationTime), type_(type), sourceId_(sourceId), message_(std::move(message)) {}

SimulationTime EventSnapshot::simulationTime() const {
    return simulationTime_;
}

SimulationTime EventSnapshot::timeSeconds() const {
    return simulationTime_;
}

EventType EventSnapshot::type() const {
    return type_;
}

MachineId EventSnapshot::sourceId() const {
    return sourceId_;
}

const std::string& EventSnapshot::message() const {
    return message_;
}

} // namespace gactorio
