#include "dto/FactorySnapshot.hpp"

#include <utility>

namespace gactorio {

FactorySnapshot::FactorySnapshot(SimulationTime simulationTime, InventorySnapshot inventory, StatisticsSnapshot statistics)
    : simulationTime_(simulationTime),
      inventory_(std::move(inventory)),
      statistics_(statistics) {}

SimulationTime FactorySnapshot::simulationTime() const {
    return simulationTime_;
}

SimulationTime FactorySnapshot::timeSeconds() const {
    return simulationTime_;
}

const InventorySnapshot& FactorySnapshot::inventory() const {
    return inventory_;
}

const StatisticsSnapshot& FactorySnapshot::statistics() const {
    return statistics_;
}

void FactorySnapshot::addProductionLine(ProductionLineSnapshot line) {
    productionLines_.push_back(std::move(line));
}

void FactorySnapshot::addEvent(EventSnapshot event) {
    events_.push_back(std::move(event));
}

const std::vector<ProductionLineSnapshot>& FactorySnapshot::productionLines() const {
    return productionLines_;
}

const std::vector<EventSnapshot>& FactorySnapshot::events() const {
    return events_;
}

} // namespace gactorio
