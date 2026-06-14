#include "model/MachineStates.hpp"

#include "model/Machine.hpp"

// =============================================================================
// MachineStates.cpp — 구체 상태 4종의 행동(update)과 이름(name) 구현
// 각 상태는 Machine 의 (friend 로 허용된) 메서드를 호출해 진행/전이를 수행한다.
// 상태 전이의 "조건과 흐름"이 바로 이 파일에 모여 있다(State 패턴의 장점: 분산 방지).
// =============================================================================

namespace gactorio {

// Idle: 평소엔 아무 것도 안 하다가, 작업이나 레시피가 생기면 Working 으로 전이.
void IdleState::update(Machine& machine, double deltaTime) {
    (void)deltaTime;
    if (machine.hasTask() || machine.recipe().has_value()) {
        machine.transitionToWorking("task available");
    }
}

std::string IdleState::name() const {
    return "Idle";
}

// Working: 매 틱 생산을 진행(여기서 손상/고장/단계완료/제품완료가 처리됨).
void WorkingState::update(Machine& machine, double deltaTime) {
    machine.advanceProduction(deltaTime);
}

std::string WorkingState::name() const {
    return "Working";
}

// Broken: 정지. update 는 아무 일도 안 한다(수리해야 벗어남).
void BrokenState::update(Machine& machine, double deltaTime) {
    (void)machine;
    (void)deltaTime;
}

std::string BrokenState::name() const {
    return "Broken";
}

// Maintenance: 매 틱 정비를 진행(시간이 차면 HP 회복 후 Working/Idle 로 복귀).
void MaintenanceState::update(Machine& machine, double deltaTime) {
    machine.advanceMaintenance(deltaTime);
}

std::string MaintenanceState::name() const {
    return "Maintenance";
}

} // namespace gactorio
