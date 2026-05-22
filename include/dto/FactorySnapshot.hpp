#pragma once

#include "dto/EventSnapshot.hpp"
#include "dto/InventorySnapshot.hpp"
#include "dto/ProductionLineSnapshot.hpp"
#include "dto/StatisticsSnapshot.hpp"
#include "common/Types.hpp"

#include <vector>

namespace gactorio {

class FactorySnapshot {
public:
    FactorySnapshot(SimulationTime simulationTime, InventorySnapshot inventory, StatisticsSnapshot statistics);

    SimulationTime simulationTime() const;
    SimulationTime timeSeconds() const;
    const InventorySnapshot& inventory() const;
    const StatisticsSnapshot& statistics() const;

    void addProductionLine(ProductionLineSnapshot line);
    void addEvent(EventSnapshot event);

    const std::vector<ProductionLineSnapshot>& productionLines() const;
    const std::vector<EventSnapshot>& events() const;

private:
    SimulationTime simulationTime_;
    InventorySnapshot inventory_;
    StatisticsSnapshot statistics_;
    std::vector<ProductionLineSnapshot> productionLines_;
    std::vector<EventSnapshot> events_;
};

} // namespace gactorio
