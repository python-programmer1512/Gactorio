#pragma once

#include <string>

namespace gactorio {

class Machine;

class MachineState {
public:
    virtual ~MachineState() = default;
    virtual void enter(Machine& machine);
    virtual void update(Machine& machine, double deltaTime) = 0;
    virtual void exit(Machine& machine);
    virtual std::string name() const = 0;
};

} // namespace gactorio

