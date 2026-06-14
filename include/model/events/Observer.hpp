#pragma once

// =============================================================================
// Observer — abstract Observer role of the Observer pattern (GoF).
// EventLogObserver and StatisticsObserver generalize it (▷) and are subscribed
// to the EventBus (Subject). See Observer_Event_Pattern_Diagram.
// =============================================================================

namespace gactorio {

struct Event;

class Observer {
public:
    virtual ~Observer() = default;
    virtual void onEvent(const Event& event) = 0;
};

} // namespace gactorio

