#pragma once

#include "common/ScenarioType.hpp"
#include "common/Types.hpp"
#include "controller/FactoryCommand.hpp"
#include "controller/SimulationHistory.hpp"
#include "dto/EventSnapshot.hpp"
#include "dto/FactorySnapshot.hpp"
#include "dto/StatisticsSnapshot.hpp"
#include "model/Factory.hpp"
#include "model/ProductCatalog.hpp"
#include "model/config/FactoryRuntimeContext.hpp"

#include <cstddef>
#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace gactorio {

class FactoryController {
public:
    FactoryController();
    explicit FactoryController(std::unique_ptr<Factory> factory);

    static FactoryController createFromConfigFile(const std::filesystem::path& path);
    static FactoryController createFromConfigString(std::string_view jsonText);

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
    FactoryCommandResult enqueueProductById(LineId lineId, ProductId productId);
    // Enqueue to whichever line currently has the smallest queue.
    // Returns the chosen LineId, or 0 if no line accepted the product.
    LineId               enqueueAutoById(ProductId productId);
    // Add a brand-new beverage line and return its LineId.
    LineId               addLine();
    // Remove a line iff it is idle (empty queue, no in-flight task).
    FactoryCommandResult removeLine(LineId id);
    FactoryCommandResult forceBreak(MachineId id);
    FactoryCommandResult repairMachine(MachineId id);       // full restore w/ delay
    FactoryCommandResult instantRepairMachine(MachineId id);
    FactoryCommandResult incrementalRepairMachine(MachineId id);  // +5 HP instant
    FactoryCommandResult restockItem(ItemType itemType, int amount);
    FactoryCommandResult restockItemById(const std::string& itemId);
    FactoryCommandResult pauseMachine(MachineId id);
    FactoryCommandResult resumeMachine(MachineId id);
    FactoryCommandResult setLineScenario(LineId lineId, ScenarioType scenario);
    FactoryCommandResult setLineScenarioById(LineId lineId, const std::string& scenarioId);
    std::optional<ScenarioType> getLineScenario(LineId lineId) const;

    FactorySnapshot getFactorySnapshot() const;
    std::vector<EventSnapshot> getEventLogs() const;
    StatisticsSnapshot getStatistics() const;
    FactorySnapshot snapshot() const;
    std::vector<ProductDefinition> availableProductDefinitions() const;
    void clearEventLog();
    bool hasRuntimeContext() const noexcept;
    const config_model::FactoryRuntimeContext* runtimeContext() const noexcept;
    const config_model::FactoryConfig* config() const noexcept;
    const config_model::DefinitionRegistry* registry() const noexcept;

    // ---- Memento (Caretaker-side façade) ---------------------------------
    void        saveCheckpoint();
    bool        undo();
    bool        canUndo() const;
    std::size_t historySize() const;

private:
    FactoryController(
        std::unique_ptr<Factory> factory,
        std::unique_ptr<config_model::FactoryRuntimeContext> runtimeContext);

    void replaceFactory(std::unique_ptr<Factory> factory);
    void replaceFactory(
        std::unique_ptr<Factory> factory,
        std::unique_ptr<config_model::FactoryRuntimeContext> runtimeContext);

    std::unique_ptr<config_model::FactoryRuntimeContext> runtimeContext_;
    std::unique_ptr<Factory> factory_;
    SimulationHistory        history_;
};

} // namespace gactorio
