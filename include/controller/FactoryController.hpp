#pragma once

#include "common/Types.hpp"
#include "controller/FactoryCommand.hpp"
#include "controller/SimulationHistory.hpp"
#include "dto/EventSnapshot.hpp"
#include "dto/FactorySnapshot.hpp"
#include "dto/StatisticsSnapshot.hpp"
#include "model/CarbonationFactory.hpp"

#include <cstddef>
#include <memory>
#include <vector>

namespace gactorio {

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
    FactoryCommandResult enqueueProduct(LineId lineId, ProductType productType);
    // Enqueue to whichever line currently has the smallest queue.
    // Returns the chosen LineId, or 0 if no line accepted the product.
    LineId               enqueueAuto(ProductType productType);
    // Add a brand-new beverage line and return its LineId.
    LineId               addLine();
    // Remove a line iff it is idle (empty queue, no in-flight task).
    FactoryCommandResult removeLine(LineId id);
    FactoryCommandResult forceBreak(MachineId id);
    FactoryCommandResult repairMachine(MachineId id);       // full restore w/ delay
    FactoryCommandResult incrementalRepairMachine(MachineId id);  // +5 HP instant
    FactoryCommandResult pauseMachine(MachineId id);
    FactoryCommandResult resumeMachine(MachineId id);

    FactorySnapshot getFactorySnapshot() const;
    std::vector<EventSnapshot> getEventLogs() const;
    StatisticsSnapshot getStatistics() const;
    FactorySnapshot snapshot() const;

    // ---- Memento (Caretaker-side façade) ---------------------------------
    void        saveCheckpoint();
    bool        undo();
    bool        canUndo() const;
    std::size_t historySize() const;

private:
    std::unique_ptr<CarbonationFactory> factory_;
    SimulationHistory                   history_;
};

} // namespace gactorio
