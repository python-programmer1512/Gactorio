#pragma once

#include "common/Types.hpp"

#include <string>

namespace gactorio {

class Event {
public:
    Event(SimulationTime simulationTime, EventType type, MachineId sourceId, std::string message);

    SimulationTime simulationTime() const;
    EventType type() const;
    MachineId sourceId() const;
    const std::string& message() const;

private:
    SimulationTime simulationTime_;
    EventType type_;
    MachineId sourceId_;
    std::string message_;
};

} // namespace gactorio
