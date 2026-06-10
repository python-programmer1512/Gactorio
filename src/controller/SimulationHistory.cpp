#include "controller/SimulationHistory.hpp"

#include <utility>

namespace gactorio {

void SimulationHistory::push(FactoryMemento m) {
    snapshots_.push_back(std::move(m));
}

bool SimulationHistory::canUndo() const {
    return !snapshots_.empty();
}

std::optional<FactoryMemento> SimulationHistory::pop() {
    if (snapshots_.empty()) return std::nullopt;
    FactoryMemento top = std::move(snapshots_.back());
    snapshots_.pop_back();
    return top;
}

void SimulationHistory::clear() {
    snapshots_.clear();
}

std::size_t SimulationHistory::size() const {
    return snapshots_.size();
}

} // namespace gactorio
