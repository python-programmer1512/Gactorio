#pragma once

#include "common/Types.hpp"

#include <string>

namespace gactorio {

class EventSnapshot {
public:
    EventSnapshot(SimulationTime simulationTime, EventType type, std::string message);

    SimulationTime simulationTime() const;
    SimulationTime timeSeconds() const;
    EventType type() const;
    const std::string& message() const;

private:
    SimulationTime simulationTime_;
    EventType type_;
    std::string message_;
};

} // namespace gactorio
