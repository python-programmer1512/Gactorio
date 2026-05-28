#pragma once

#include "common/SimClock.hpp"
#include "common/Types.hpp"
#include "model/Inventory.hpp"
#include "model/ProductCatalog.hpp"
#include "model/ProductionLine.hpp"
#include "model/events/EventBus.hpp"
#include "model/events/EventLogObserver.hpp"
#include "model/events/StatisticsObserver.hpp"
#include "model/memento/FactoryMemento.hpp"

#include <memory>
#include <vector>

namespace gactorio {

enum class ProductionRequestResult {
    Success,
    LineNotFound,
    InvalidRequest,
    InsufficientMaterials
};

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
    const ProductCatalog& productCatalog() const;
    ProductCatalog& productCatalog();
    EventBus& eventBus();
    const EventBus& eventBus() const;
    const SimClock& clock() const;

    void addProductionLine(ProductionLine line);
    ProductionRequestResult enqueueProduct(LineId lineId, std::shared_ptr<Product> product);
    ProductionRequestResult enqueueProduct(LineId lineId, std::unique_ptr<Product> product);
    ProductionLine* findProductionLine(LineId id);
    const ProductionLine* findProductionLine(LineId id) const;
    Machine* findMachine(MachineId id);
    SimulationTime update(double realDeltaTime);
    void pauseClock();
    void resumeClock();
    void resetClock();
    void stopClock();
    void setClockSpeed(double speedMultiplier);
    FactoryMemento createMemento() const;
    void restoreFromMemento(const FactoryMemento& state);

protected:
    EventLog& mutableEventLog();
    Statistics& mutableStatistics();
    virtual std::vector<RecipeMemento> exportRecipeStates() const;
    virtual void restoreRecipeStates(const std::vector<RecipeMemento>& recipes);

private:
    void rebuildMachineRegistry();
    void reconnectEventBus();

    SimClock clock_;
    ProductCatalog productCatalog_;
    Inventory inventory_;
    std::vector<ProductionLine> productionLines_;
    std::vector<Machine*> machines_;
    EventBus eventBus_;
    EventLogObserver eventLog_;
    StatisticsObserver statistics_;
};

} // namespace gactorio
