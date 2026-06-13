#pragma once

// =============================================================================
// MachineStates — MachineState 를 상속한 구체 상태 4종
// -----------------------------------------------------------------------------
//   IdleState        : 유휴. 작업/레시피가 생기면 Working 으로 전이.
//   WorkingState     : 가동. advanceProduction()으로 공정 진행 + 무작위 HP 손상.
//   BrokenState      : 고장. update 는 아무것도 안 함(수리 전까지 정지).
//   MaintenanceState : 정비. advanceMaintenance()로 시간이 차면 HP 회복.
//
// 전이 상세는 Machine_State_Diagram.drawio(상태 다이어그램) 및 MachineStates.cpp 참고.
// 모두 final + override 로 작은 상태 클래스. 데이터 멤버는 없다(행동만 정의).
// =============================================================================

#include "model/MachineState.hpp"

namespace gactorio {

class IdleState final : public MachineState {
public:
    void update(Machine& machine, double deltaTime) override;  // 작업 있으면 Working 전이
    std::string name() const override;                         // "Idle"
};

class WorkingState final : public MachineState {
public:
    void update(Machine& machine, double deltaTime) override;  // 생산 진행 위임
    std::string name() const override;                         // "Working"
};

class BrokenState final : public MachineState {
public:
    void update(Machine& machine, double deltaTime) override;  // no-op(정지)
    std::string name() const override;                         // "Broken"
};

class MaintenanceState final : public MachineState {
public:
    void update(Machine& machine, double deltaTime) override;  // 수리 진행 위임
    std::string name() const override;                         // "Maintenance"
};

} // namespace gactorio
