#include "controller/SimulationHistory.hpp"

#include <cassert>

namespace {

gactorio::FactoryMemento makeState(double time, int water) {
    gactorio::FactoryMemento state;
    state.clock.currentTime = time;
    state.inventory.items[gactorio::ItemType::Water] = water;
    return state;
}

} // namespace

int main() {
    gactorio::SimulationHistory history(2);

    assert(!history.canUndo());
    assert(!history.canRedo());
    assert(!history.undo(makeState(0.0, 0)).has_value());
    assert(!history.redo(makeState(0.0, 0)).has_value());

    history.record(makeState(1.0, 10));
    history.record(makeState(2.0, 20));
    assert(history.canUndo());
    assert(!history.canRedo());

    auto previous = history.undo(makeState(3.0, 30));
    assert(previous.has_value());
    assert(previous->clock.currentTime == 2.0);
    assert(previous->inventory.items[gactorio::ItemType::Water] == 20);
    assert(history.canUndo());
    assert(history.canRedo());

    previous = history.undo(*previous);
    assert(previous.has_value());
    assert(previous->clock.currentTime == 1.0);
    assert(!history.canUndo());
    assert(history.canRedo());

    auto next = history.redo(*previous);
    assert(next.has_value());
    assert(next->clock.currentTime == 2.0);
    assert(history.canUndo());
    assert(history.canRedo());

    history.record(makeState(4.0, 40));
    assert(history.canUndo());
    assert(!history.canRedo());

    history.record(makeState(5.0, 50));
    previous = history.undo(makeState(6.0, 60));
    assert(previous.has_value());
    assert(previous->clock.currentTime == 5.0);

    previous = history.undo(*previous);
    assert(previous.has_value());
    assert(previous->clock.currentTime == 4.0);
    assert(!history.canUndo());

    history.clear();
    assert(!history.canUndo());
    assert(!history.canRedo());

    return 0;
}
