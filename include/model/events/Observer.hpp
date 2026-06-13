#pragma once

// =============================================================================
// Observer — Observer 패턴의 추상 구독자 인터페이스
// -----------------------------------------------------------------------------
// EventBus 에 등록되는 모든 구독자가 구현하는 작은 인터페이스(ISP). onEvent() 하나만
// 가진다. 구체 구독자: EventLogObserver(이벤트 저장), StatisticsObserver(집계).
// 발행자는 구독자가 이벤트를 "어떻게" 처리하는지 전혀 모른다(느슨한 결합).
// =============================================================================

namespace gactorio {

struct Event;   // 전방 선언(onEvent 인자 타입)

class Observer {
public:
    virtual ~Observer() = default;
    virtual void onEvent(const Event& event) = 0;   // 이벤트 수신 시 호출(필수 구현)
};

} // namespace gactorio
