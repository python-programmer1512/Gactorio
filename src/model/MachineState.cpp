#include "model/MachineState.hpp"

// =============================================================================
// MachineState.cpp — 추상 상태의 기본 훅 구현
// enter/exit 는 대부분의 상태에서 할 일이 없으므로 기본 구현을 no-op 으로 둔다.
// (필요한 상태만 override 하면 됨.) update/name 은 순수 가상이라 여기 구현 없음.
// =============================================================================

namespace gactorio {

void MachineState::enter(Machine& machine) {
    (void)machine;   // 기본: 진입 시 아무 일도 안 함
}

void MachineState::exit(Machine& machine) {
    (void)machine;   // 기본: 탈출 시 아무 일도 안 함
}

} // namespace gactorio
