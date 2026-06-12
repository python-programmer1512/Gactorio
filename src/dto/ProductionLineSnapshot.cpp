#include "dto/ProductionLineSnapshot.hpp"

#include <utility>

namespace gactorio {

ProductionLineSnapshot::ProductionLineSnapshot(
    ProductionLineId id,
    std::string name,
    std::size_t queueLength,
    std::string currentTaskName,
    double currentTaskProgress)
    : id_(id),
      name_(std::move(name)),
      queueLength_(queueLength),
      currentTaskName_(std::move(currentTaskName)),
      currentTaskProgress_(currentTaskProgress) {}

ProductionLineId ProductionLineSnapshot::id() const {
    return id_;
}

const std::string& ProductionLineSnapshot::name() const {
    return name_;
}

std::size_t ProductionLineSnapshot::queueLength() const {
    return queueLength_;
}

const std::string& ProductionLineSnapshot::currentTaskName() const {
    return currentTaskName_;
}

double ProductionLineSnapshot::currentTaskProgress() const {
    return currentTaskProgress_;
}

void ProductionLineSnapshot::setCurrentTaskProgress(double progress) {
    currentTaskProgress_ = progress;
}

void ProductionLineSnapshot::addMachine(MachineSnapshot machine) {
    machines_.push_back(std::move(machine));
}

const std::vector<MachineSnapshot>& ProductionLineSnapshot::machines() const {
    return machines_;
}

} // namespace gactorio
