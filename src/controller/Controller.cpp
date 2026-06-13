#include "controller/Controller.h"

// Only this .cpp file knows about the gactorio model. Translation between
// view-friendly types (ctrl::*) and model types (gactorio::*) lives here.
#include "common/ScenarioType.hpp"
#include "common/Types.hpp"
#include "controller/FactoryController.hpp"
#include "model/Item.hpp"
#include "model/ProductCatalog.hpp"

#include <sstream>
#include <stdexcept>

namespace ctrl {
namespace {

gactorio::ProductId toModelId(ProductKind k) {
    return static_cast<gactorio::ProductId>(k);
}

const char* stateName(gactorio::MachineStatus s) {
    using S = gactorio::MachineStatus;
    switch (s) {
    case S::Idle:        return "Idle";
    case S::Working:     return "Working";
    case S::Paused:      return "Paused";
    case S::Blocked:     return "Blocked";
    case S::Broken:      return "Broken";
    case S::Maintenance: return "Maintenance";
    }
    return "Unknown";
}

const char* eventName(gactorio::EventType t) {
    using E = gactorio::EventType;
    switch (t) {
    case E::Info:             return "Info";
    case E::TaskEnqueued:     return "TaskEnqueued";
    case E::TaskStarted:      return "TaskStarted";
    case E::StepCompleted:    return "StepCompleted";
    case E::ProductCompleted: return "ProductCompleted";
    case E::MachineBroken:    return "MachineBroken";
    case E::MachineRepaired:  return "MachineRepaired";
    case E::StateChanged:     return "StateChanged";
    case E::InputsConsumed:   return "InputsConsumed";
    case E::MachinePaused:    return "MachinePaused";
    }
    return "Unknown";
}

// Gactorio stores inventory IDs as numeric strings (item-types 1-5,
// product-ids 101-103). Translate them to human-readable labels.
ItemId parseInventoryId(const std::string& numeric) {
    try {
        return static_cast<ItemId>(std::stoul(numeric));
    } catch (const std::exception&) {
        return 0;
    }
}

bool isProductInventoryId(ItemId id) {
    return gactorio::findProductDefinition(static_cast<gactorio::ProductId>(id)) != nullptr;
}

std::string humanizeInventoryId(ItemId id) {
    if (const auto* product = gactorio::findProductDefinition(static_cast<gactorio::ProductId>(id))) {
        return product->name;
    }
    return gactorio::ItemTypeName::get(static_cast<gactorio::ItemType>(id));
}

std::string formatRequirements(const gactorio::ProductDefinition& definition) {
    std::ostringstream out;
    bool first = true;
    for (const auto& requirement : definition.requirements) {
        if (!first) {
            out << ", ";
        }
        first = false;
        out << gactorio::ItemTypeName::get(requirement.itemType())
            << " x" << requirement.quantity();
    }
    return out.str();
}

} // namespace

// =============================================================================
// PImpl
// =============================================================================
struct Controller::Impl {
    gactorio::FactoryController backend;
    mutable FactoryView         cached;
    std::vector<ProductOption>  products;
    mutable bool                dirty = true;

    Impl() {
        for (const auto& definition : gactorio::productDefinitions()) {
            products.push_back({
                static_cast<ProductId>(definition.id),
                definition.key,
                definition.name,
                definition.tier,
                definition.totalDurationSeconds,
                formatRequirements(definition)
            });
        }
    }

    void rebuild() const {
        const auto snap = backend.snapshot();
        cached = {};
        cached.simulationTime = snap.timeSeconds();

        const auto& s = snap.statistics();
        cached.stats = {
            s.startedTaskEvents(),
            s.completedStepEvents(),
            s.completedProductEvents(),
            s.brokenMachineEvents(),
            s.repairedMachineEvents(),
            s.stateChangedEvents()
        };

        for (const auto& line : snap.productionLines()) {
            LineView lv;
            lv.id                  = static_cast<LineId>(line.id());
            lv.name                = line.name();
            lv.queueLength         = line.queueLength();
            lv.currentTaskName     = line.currentTaskName();
            lv.currentTaskProgress = line.currentTaskProgress();
            lv.scenarioId          = line.scenarioId();
            lv.scenarioName        = line.scenarioName();
            lv.queueCapacity       = line.queueCapacity();
            lv.droppedTaskCount    = line.droppedTaskCount();
            bool busy = lv.queueLength > 0 || !lv.currentTaskName.empty();
            for (const auto& m : line.machines()) {
                lv.machines.push_back({
                    static_cast<MachineId>(m.id()),
                    m.name(), m.typeName(),
                    stateName(m.status()), m.progress(), m.health()
                });
                if (m.status() == gactorio::MachineStatus::Working ||
                    m.status() == gactorio::MachineStatus::Maintenance) {
                    busy = true;
                }
            }
            lv.isRemovable = !busy;
            cached.lines.push_back(std::move(lv));
        }

        for (const auto& e : snap.events()) {
            cached.events.push_back({e.timeSeconds(), eventName(e.type()), e.message()});
        }

        for (const auto& entry : snap.inventory().items()) {
            const auto id = parseInventoryId(entry.id());
            cached.inventory.push_back({
                id,
                humanizeInventoryId(id),
                entry.quantity(),
                isProductInventoryId(id)
            });
        }

        dirty = false;
    }
};

// =============================================================================
// Public API
// =============================================================================
Controller::Controller()  : m_impl(std::make_unique<Impl>()) {}
Controller::~Controller() = default;

void Controller::tick(double dt)               { m_impl->backend.tick(dt);               m_impl->dirty = true; }
void Controller::pause()                       { m_impl->backend.pauseSimulation();      m_impl->dirty = true; }
void Controller::resume()                      { m_impl->backend.resumeSimulation();     m_impl->dirty = true; }
void Controller::reset()                       { m_impl->backend.resetSimulation();      m_impl->dirty = true; }
void Controller::setSpeed(double mult)         { m_impl->backend.setSimulationSpeed(mult); }

bool Controller::enqueue(LineId line, ProductKind p) {
    return enqueueProduct(line, static_cast<ProductId>(toModelId(p)));
}

bool Controller::enqueueProduct(LineId line, ProductId p) {
    m_impl->dirty = true;
    return m_impl->backend.enqueueProductById(line, static_cast<gactorio::ProductId>(p))
        == gactorio::FactoryCommandResult::Success;
}
LineId Controller::enqueueAuto(ProductKind p) {
    return enqueueAutoProduct(static_cast<ProductId>(toModelId(p)));
}
LineId Controller::enqueueAutoProduct(ProductId p) {
    m_impl->dirty = true;
    return static_cast<LineId>(m_impl->backend.enqueueAutoById(static_cast<gactorio::ProductId>(p)));
}
LineId Controller::addLine() {
    m_impl->dirty = true;
    return static_cast<LineId>(m_impl->backend.addLine());
}
bool Controller::removeLine(LineId id) {
    m_impl->dirty = true;
    return m_impl->backend.removeLine(id)
        == gactorio::FactoryCommandResult::Success;
}
bool Controller::breakMachine(MachineId id) {
    m_impl->dirty = true;
    return m_impl->backend.forceBreak(id)
        == gactorio::FactoryCommandResult::Success;
}
bool Controller::repair(MachineId id) {
    m_impl->dirty = true;
    return m_impl->backend.incrementalRepairMachine(id)
        == gactorio::FactoryCommandResult::Success;
}
bool Controller::restockItem(ItemId id) {
    m_impl->dirty = true;
    return m_impl->backend.restockItem(static_cast<gactorio::ItemType>(id), 5)
        == gactorio::FactoryCommandResult::Success;
}
bool Controller::repairAll(MachineId id) {
    m_impl->dirty = true;
    return m_impl->backend.repairMachine(id)
        == gactorio::FactoryCommandResult::Success;
}

bool Controller::setLineScenario(LineId line, const std::string& scenarioId) {
    m_impl->dirty = true;
    return m_impl->backend.setLineScenarioById(line, scenarioId)
        == gactorio::FactoryCommandResult::Success;
}

const FactoryView& Controller::snapshot() const {
    if (m_impl->dirty) m_impl->rebuild();
    return m_impl->cached;
}

std::string Controller::getLineScenario(LineId line) const {
    const auto scenario = m_impl->backend.getLineScenario(line);
    if (!scenario.has_value()) {
        return "";
    }
    return gactorio::scenarioTypeToString(*scenario);
}

const std::vector<ProductOption>& Controller::products() const {
    return m_impl->products;
}

// ---- Memento façade ---------------------------------------------------------
void Controller::saveCheckpoint() {
    m_impl->backend.saveCheckpoint();
}

bool Controller::undo() {
    if (!m_impl->backend.undo()) return false;
    m_impl->dirty = true;
    return true;
}

bool Controller::canUndo() const {
    return m_impl->backend.canUndo();
}

std::size_t Controller::historySize() const {
    return m_impl->backend.historySize();
}

} // namespace ctrl
