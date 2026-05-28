#pragma once

#include "common/Types.hpp"
#include "controller/FactoryCommand.hpp"
#include "controller/SimulationHistory.hpp"
#include "dto/EventSnapshot.hpp"
#include "dto/FactorySnapshot.hpp"
#include "dto/StatisticsSnapshot.hpp"
#include "model/CarbonationFactory.hpp"

#include <memory>
#include <vector>

namespace gactorio {

class SimulationHistoryStatus {
public:
    SimulationHistoryStatus(bool canUndo, bool canRedo)
        : canUndo_(canUndo), canRedo_(canRedo) {}

    bool canUndo() const {
        return canUndo_;
    }

    bool canRedo() const {
        return canRedo_;
    }

private:
    bool canUndo_;
    bool canRedo_;
};

class FactoryController {
public:
    FactoryController();

    void createDefaultCarbonationFactory();
    void reset();
    void tick(double deltaTime);
    void startSimulation();
    void pauseSimulation();
    void resetSimulation();
    void resumeSimulation();
    void resetSimulationClock();
    void stopSimulation();
    void setSpeed(double speedMultiplier);
    void setSimulationSpeed(double speedMultiplier);
    FactoryCommandResult enqueueProduct(LineId lineId, ProductId productId);
    FactoryCommandResult forceBreak(MachineId id);
    FactoryCommandResult repairMachine(MachineId id);
    FactoryCommandResult pauseMachine(MachineId id);
    FactoryCommandResult resumeMachine(MachineId id);
    FactoryCommandResult saveState();
    FactoryCommandResult undo();
    FactoryCommandResult redo();
    bool canUndo() const;
    bool canRedo() const;
    void clearHistory();
    SimulationHistoryStatus getHistoryStatus() const;
    const ProductCatalog& productCatalog() const;
    ProductCatalog& productCatalog();

    FactorySnapshot getFactorySnapshot() const;
    std::vector<EventSnapshot> getEventLogs() const;
    StatisticsSnapshot getStatistics() const;
    FactorySnapshot snapshot() const;

private:
    void recordCurrentState();

    std::unique_ptr<CarbonationFactory> factory_;
    SimulationHistory history_;
    bool suppressAutoHistory_ = false;
};

} // namespace gactorio
