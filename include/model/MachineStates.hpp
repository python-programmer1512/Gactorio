#pragma once

#include "common/Types.hpp"
#include "model/MachineState.hpp"

#include <memory>

namespace gactorio {

std::unique_ptr<MachineState> makeMachineState(MachineStatus status);

class IdleState final : public MachineState {
public:
    void update(Machine& machine, double deltaTime) override;
    std::string name() const override;
};

class WorkingState final : public MachineState {
public:
    void update(Machine& machine, double deltaTime) override;
    std::string name() const override;
};

class PausedState final : public MachineState {
public:
    void update(Machine& machine, double deltaTime) override;
    std::string name() const override;
};

class BrokenState final : public MachineState {
public:
    void update(Machine& machine, double deltaTime) override;
    std::string name() const override;
};

class MaintenanceState final : public MachineState {
public:
    void update(Machine& machine, double deltaTime) override;
    std::string name() const override;
};

} // namespace gactorio
