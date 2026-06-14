#include "dto/ProductionLineSnapshot.hpp"

#include <utility>

// =============================================================================
// ProductionLineSnapshot.cpp — 라인 DTO 구현(기계 스냅샷 목록을 누적 보관).
// =============================================================================

namespace gactorio {

ProductionLineSnapshot::ProductionLineSnapshot(
    ProductionLineId id,
    std::string name,
    std::size_t queueLength,
    std::string currentTaskName,
    double currentTaskProgress,
    std::string scenarioId,
    std::string scenarioName,
    std::size_t queueCapacity,
    std::size_t droppedTaskCount)
    : id_(id),
      name_(std::move(name)),
      queueLength_(queueLength),
      currentTaskName_(std::move(currentTaskName)),
      currentTaskProgress_(currentTaskProgress),
      scenarioId_(std::move(scenarioId)),
      scenarioName_(std::move(scenarioName)),
      queueCapacity_(queueCapacity),
      droppedTaskCount_(droppedTaskCount) {}

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

const std::string& ProductionLineSnapshot::scenarioId() const {
    return scenarioId_;
}

const std::string& ProductionLineSnapshot::scenarioName() const {
    return scenarioName_;
}

std::size_t ProductionLineSnapshot::queueCapacity() const {
    return queueCapacity_;
}

std::size_t ProductionLineSnapshot::droppedTaskCount() const {
    return droppedTaskCount_;
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
