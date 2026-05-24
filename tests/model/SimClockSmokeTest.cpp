#include "common/SimClock.hpp"

#include <cassert>

int main() {
    gactorio::SimClock clock;

    assert(clock.now() == 0.0);
    assert(clock.speedMultiplier() == 1.0);
    assert(!clock.isPaused());
    assert(!clock.isStopped());

    assert(clock.update(2.0) == 2.0);
    assert(clock.now() == 2.0);
    assert(clock.getDeltaTime() == 2.0);

    clock.setSpeed(2.5);
    assert(clock.update(2.0) == 5.0);
    assert(clock.now() == 7.0);

    clock.pause();
    assert(clock.isPaused());
    assert(clock.update(10.0) == 0.0);
    assert(clock.now() == 7.0);

    clock.resume();
    assert(!clock.isPaused());
    assert(!clock.isStopped());
    assert(clock.update(1.0) == 2.5);
    assert(clock.now() == 9.5);

    clock.stop();
    assert(clock.isStopped());
    assert(clock.update(3.0) == 0.0);
    assert(clock.now() == 9.5);

    clock.resume();
    assert(clock.update(1.0) == 2.5);
    assert(clock.now() == 12.0);

    clock.setSpeed(-1.0);
    assert(clock.speedMultiplier() == 0.0);
    assert(clock.update(5.0) == 0.0);
    assert(clock.now() == 12.0);

    clock.reset();
    assert(clock.now() == 0.0);
    assert(clock.getDeltaTime() == 0.0);
    assert(clock.speedMultiplier() == 1.0);
    assert(!clock.isPaused());
    assert(!clock.isStopped());

    return 0;
}
