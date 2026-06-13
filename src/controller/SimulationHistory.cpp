#include "controller/SimulationHistory.hpp"

#include <utility>

// =============================================================================
// SimulationHistory.cpp — Caretaker 구현(단순 스택). 메멘토 내용은 보지 않는다.
// =============================================================================

namespace gactorio {

// 스냅샷 적재(가장 뒤가 최신).
void SimulationHistory::push(FactoryMemento m) {
    snapshots_.push_back(std::move(m));
}

bool SimulationHistory::canUndo() const {
    return !snapshots_.empty();
}

// 최신 스냅샷을 꺼내 반환(없으면 nullopt). LIFO = undo 동작.
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
