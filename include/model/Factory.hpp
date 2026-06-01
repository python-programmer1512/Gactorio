#pragma once

#include "common/SimClock.hpp"
#include "common/Types.hpp"
#include "model/Inventory.hpp"
#include "model/ProductionLine.hpp"
#include "model/events/EventBus.hpp"
#include "model/events/EventLogObserver.hpp"
#include "model/events/StatisticsObserver.hpp"
#include "model/memento/FactoryMemento.hpp"

#include <memory>
#include <vector>

namespace gactorio {

class Factory {
public:
    Factory();
    virtual ~Factory() = default;

    SimulationTime simulationTime() const;
    const Inventory& inventory() const;
    Inventory& inventory();
    const std::vector<ProductionLine>& productionLines() const;
    const std::vector<Machine*>& machines() const;
    const EventLog& eventLog() const;
    const Statistics& statistics() const;
    EventBus& eventBus();
    const EventBus& eventBus() const;
    const SimClock& clock() const;

    void addProductionLine(ProductionLine line);
    ProductionLine* findProductionLine(LineId id);
    const ProductionLine* findProductionLine(LineId id) const;
    Machine* findMachine(MachineId id);
    SimulationTime update(double realDeltaTime);
    void pauseClock();
    void resumeClock();
    void resetClock();
    void stopClock();
    void setClockSpeed(double speedMultiplier);

    // ---- Memento (Originator side) ---------------------------------------
    // Capture the current factory state as an opaque snapshot.
    FactoryMemento createMemento() const;
    // Apply a previously captured snapshot. In-flight tasks are not
    // restored (machines reset to Idle with their old HP); the line
    // queues are repopulated from the snapshot so work can resume.
    void           restoreFromMemento(const FactoryMemento& memento);

protected:
    EventLog& mutableEventLog();
    Statistics& mutableStatistics();

    // Subclass-supplied product builder. The default Factory cannot rebuild
    // products from IDs alone, so this is virtual; CarbonationFactory
    // overrides it to dispatch to VoltzClassic / HyperBolt / AuroraZero.
    virtual std::shared_ptr<Product> createProductById(ProductId id) const;

private:
    SimClock clock_;
    Inventory inventory_;
    std::vector<ProductionLine> productionLines_;
    std::vector<Machine*> machines_;
    EventBus eventBus_;
    EventLogObserver eventLog_;
    StatisticsObserver statistics_;
};

} // namespace gactorio
