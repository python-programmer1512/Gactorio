#pragma once

#include "common/Types.hpp"
#include "dto/MachineSnapshot.hpp"

#include <cstddef>
#include <string>
#include <vector>

namespace gactorio {

class ProductionLineSnapshot {
public:
    ProductionLineSnapshot(
        ProductionLineId id,
        std::string name,
        std::size_t queueLength = 0,
        std::string currentTaskName = "",
        double currentTaskProgress = 0.0);

    ProductionLineId id() const;
    const std::string& name() const;
    std::size_t queueLength() const;
    const std::string& currentTaskName() const;
    double currentTaskProgress() const;
    void setCurrentTaskProgress(double progress);
    void addMachine(MachineSnapshot machine);
    const std::vector<MachineSnapshot>& machines() const;

private:
    ProductionLineId id_;
    std::string name_;
    std::size_t queueLength_;
    std::string currentTaskName_;
    double currentTaskProgress_;
    std::vector<MachineSnapshot> machines_;
};

using LineSnapshot = ProductionLineSnapshot;

} // namespace gactorio
