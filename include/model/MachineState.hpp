#pragma once

// =============================================================================
// MachineState — State 패턴의 추상 "상태" 인터페이스
// -----------------------------------------------------------------------------
// Machine 은 현재 상태 객체에 행동을 위임한다. 각 구체 상태(Idle/Working/Broken/
// Maintenance)는 update() 안에서 자기 상태에 맞는 행동을 하고, 필요하면 Machine 의
// 전이 함수를 호출해 다음 상태로 넘긴다. 이렇게 하면 상태 분기가 Machine 코드 전체에
// 퍼지지 않고 각 상태 클래스 안에 캡슐화된다.
//
// 작은 인터페이스(ISP): enter/update/exit/name 4개만 가진다.
// enter/exit 는 기본 구현(아무것도 안 함)을 제공하고, update/name 은 순수 가상.
// =============================================================================

#include <string>

namespace gactorio {

class Machine;   // 상태는 Machine 을 참조로만 다룸(상호 헤더 의존 회피용 전방 선언)

class MachineState {
public:
    virtual ~MachineState() = default;
    virtual void enter(Machine& machine);                       // 상태 진입 훅(기본: no-op)
    virtual void update(Machine& machine, double deltaTime) = 0; // 매 틱 행동(필수 구현)
    virtual void exit(Machine& machine);                        // 상태 탈출 훅(기본: no-op)
    virtual std::string name() const = 0;                       // 상태 이름(필수 구현)
};

} // namespace gactorio
