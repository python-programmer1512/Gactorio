#pragma once

// =============================================================================
// Factory — the simulation aggregate root, and the Originator of the Memento
// pattern.
//
// UML references:
//   * BE_Overall_Class_Diagram  — Factory OWNS (composition ◆): Inventory,
//     the ProductionLines (by-value vector), the EventBus, the two Observers
//     (EventLogObserver, StatisticsObserver) and the SimClock. It also keeps a
//     non-owning Machine* cache (aggregation ◇) flattened from the lines.
//     CarbonationFactory generalizes Factory (▷).
//   * Memento_Pattern_Diagram   — createMemento()/restoreFromMemento() make
//     Factory the Originator; FactoryMemento is the Memento product.
//
// Relationships:
//   has-a : Inventory, vector<ProductionLine>, EventBus, two Observers,
//           SimClock (all owned); vector<Machine*> cache (non-owning)
//   is-a  : CarbonationFactory is-a Factory
// =============================================================================

#include "common/SimClock.hpp"
#include "common/Types.hpp"
#include "model/Inventory.hpp"
#include "model/ProductionLine.hpp"
#include "model/events/EventBus.hpp"
#include "model/events/EventLogObserver.hpp"
#include "model/events/StatisticsObserver.hpp"
#include "model/memento/FactoryMemento.hpp"

#include <memory>
#include <optional>
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
    bool removeProductionLine(LineId id);
    std::shared_ptr<Product> createProductForQueue(ProductId id) const;
    EnqueueResult enqueueProduct(LineId lineId, std::shared_ptr<Product> product);
    bool restockItem(ItemType itemType, int amount);
    ProductionLine* findProductionLine(LineId id);
    const ProductionLine* findProductionLine(LineId id) const;
    Machine* findMachine(MachineId id);
    bool setLineScenario(LineId lineId, ScenarioType scenario);
    std::optional<ScenarioType> getLineScenario(LineId lineId) const;
    void clearEventLog();
    SimulationTime update(double realDeltaTime);
    void pauseClock();
    void resumeClock();
    void resetClock();
    void stopClock();
    void setClockSpeed(double speedMultiplier);

    // ---- Memento (Originator side) ---------------------------------------
    // Capture the current factory state as an opaque snapshot.
    virtual FactoryMemento createMemento() const;
    // Apply a previously captured snapshot. In-flight tasks are not
    // restored (machines reset to Idle with their old HP); the line
    // queues are repopulated from the snapshot so work can resume.
    virtual void   restoreFromMemento(const FactoryMemento& memento);

protected:
    EventLog& mutableEventLog();
    Statistics& mutableStatistics();

    // Subclass-supplied product builder. The default Factory cannot rebuild
    // products from IDs alone, so this is virtual; CarbonationFactory delegates
    // to the product catalog used by the controller and view.
    virtual std::shared_ptr<Product> createProductById(ProductId id) const;
    virtual std::optional<ProductionLine> createLineForMemento(const LineMemento& memento) const;

private:
    void rebuildMachineCache();

    SimClock clock_;
    Inventory inventory_;
    std::vector<ProductionLine> productionLines_;
    std::vector<Machine*> machines_;
    EventBus eventBus_;
    EventLogObserver eventLog_;
    StatisticsObserver statistics_;
};

} // namespace gactorio
