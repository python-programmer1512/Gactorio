#pragma once

#include "common/Types.hpp"

#include <string>

namespace gactorio {

class MachineSnapshot {
public:
    MachineSnapshot(
        MachineId id,
        std::string name,
        std::string typeName,
        MachineStatus status,
        std::string stateName,
        double progress,
        double health);

    MachineId id() const;
    const std::string& name() const;
    const std::string& typeName() const;
    MachineStatus status() const;
    const std::string& stateName() const;
    double progress() const;
    double health() const;

private:
    MachineId id_;
    std::string name_;
    std::string typeName_;
    MachineStatus status_;
    std::string stateName_;
    double progress_;
    double health_;
};

} // namespace gactorio
