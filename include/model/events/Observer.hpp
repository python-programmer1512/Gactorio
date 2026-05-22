#pragma once

namespace gactorio {

struct Event;

class Observer {
public:
    virtual ~Observer() = default;
    virtual void onEvent(const Event& event) = 0;
};

} // namespace gactorio

