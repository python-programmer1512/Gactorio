#pragma once

#include "common/Types.hpp"

#include <string>

namespace gactorio {

class EventSnapshot {
public:
    EventSnapshot(SimulationTime simulationTime, EventType type, MachineId sourceId, std::string message);

    SimulationTime simulationTime() const;
    SimulationTime timeSeconds() const;
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
