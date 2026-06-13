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
        double currentTaskProgress = 0.0,
        std::string scenarioId = "normal-flow",
        std::string scenarioName = "Normal Flow",
        std::size_t queueCapacity = 0,
        std::size_t droppedTaskCount = 0);

    ProductionLineId id() const;
    const std::string& name() const;
    std::size_t queueLength() const;
    const std::string& currentTaskName() const;
    double currentTaskProgress() const;
    const std::string& scenarioId() const;
    const std::string& scenarioName() const;
    std::size_t queueCapacity() const;
    std::size_t droppedTaskCount() const;
    void setCurrentTaskProgress(double progress);
    void addMachine(MachineSnapshot machine);
    const std::vector<MachineSnapshot>& machines() const;

private:
    ProductionLineId id_;
    std::string name_;
    std::size_t queueLength_;
    std::string currentTaskName_;
    double currentTaskProgress_;
    std::string scenarioId_;
    std::string scenarioName_;
    std::size_t queueCapacity_;
    std::size_t droppedTaskCount_;
    std::vector<MachineSnapshot> machines_;
};

using LineSnapshot = ProductionLineSnapshot;

} // namespace gactorio
