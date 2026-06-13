#pragma once

// =============================================================================
// Machine — 모든 생산 설비의 추상 기반 클래스 (이 프로젝트의 다형성 핵심)
// -----------------------------------------------------------------------------
// 설계 요지:
//   * Factory/ProductionLine 은 구체 기계 타입을 모른 채 Machine* 기반 포인터로만
//     update() 를 호출한다 → 시뮬레이션 루프에 if/else/dynamic_cast 가 없다(과제 요구).
//   * 기계의 "상태별 행동"은 State 패턴으로 분리한다: Machine 은 현재 상태 객체
//     (state_: unique_ptr<MachineState>) 에 update 를 위임하고, 상태 객체가 다음
//     전이를 결정한다. 덕분에 Machine::update 가 거대한 switch 문이 되지 않는다.
//   * 도메인 이벤트는 Observer 패턴으로 발행한다: notify() → EventBus → 구독자들.
//
// is-a:   MixingStation/QualityStation/BottlingStation/PackagingStation : Machine
// has-a:  Machine ◆ MachineState(state_), ◆ optional<Recipe>(recipe_),
//         ◇ ProductionTask(task_, 공유), → EventBus*(비소유 참조)
//
// 캡슐화: 모든 데이터 멤버 private. HP/진행률/상태는 메서드로만 바뀐다.
// 상태 객체들은 Machine 의 private 전이 함수를 호출해야 하므로 friend 로 선언한다.
// =============================================================================

#include "common/Types.hpp"
#include "model/ProductionTask.hpp"
#include "model/Recipe.hpp"
#include "model/events/Event.hpp"
#include "model/events/EventBus.hpp"

#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace gactorio {

// 전방 선언: 상태 클래스들(서로 friend 관계).
class MachineState;
class IdleState;
class WorkingState;
class BrokenState;
class MaintenanceState;

class Machine {
public:
    Machine(MachineId id, std::string name);   // 간단 생성(기본 스펙)
    virtual ~Machine();

    // ---- 조회용 접근자 -----------------------------------------------------
    MachineId id() const;
    const std::string& name() const;
    double progress() const;                       // 현재 진행률(0~1, 경로 기준)
    std::string stateName() const;                 // 현재 상태 이름("Idle" 등)
    bool hasTask() const;                          // 진행 중 작업 보유 여부
    const ProductionTask* currentTask() const;     // 현재 작업(없으면 nullptr)
    const std::optional<Recipe>& recipe() const;   // 할당된 레시피(옵션)

    // ---- 명령용 메서드 -----------------------------------------------------
    bool assignTask(std::shared_ptr<ProductionTask> task);  // 작업 배정 시도
    void setRecipe(const Recipe& recipe);                   // 레시피 설정
    void setEventBus(EventBus* eventBus);                   // 이벤트 버스 연결
    virtual void update(double deltaTime);                  // 매 틱 호출(상태에 위임)
    bool canAcceptTask() const;                             // 작업 수락 가능?
    bool canProcess(MachineRole role) const;                // 해당 역할 처리 가능?
    void forceBreak();                                      // (디버그/테스트) 강제 고장
    // repair(): 긴 정비 루틴 시작 → 완료 시 HP 전량 회복.
    void repair();
    // incrementalRepair(): 소량(config::kIncrementalRepairHp) HP 즉시 회복.
    // 단, 고장(Broken) 상태는 되살리지 못함 — 그건 repair()/repairAll 담당.
    void incrementalRepair();

    // ---- Memento 복원 지원 (Factory::restoreFromMemento 가 사용) -----------
    // 진행 중 작업을 버리고, 진행률 0, HP 덮어쓰기, 주어진 상태로 강제 전이.
    void resetForRestore(double newHealth, MachineStatus newStatus);

    // get* 형태 접근자(스냅샷 생성/메멘토에서 사용).
    MachineId getId() const;
    const std::string& getName() const;
    MachineStatus getStatus() const;
    double getProgress() const;
    double getHealth() const;
    double getProcessingSpeed() const;
    double getBreakdownProbability() const;
    void pause();    // 일시정지(Idle로) — 고장/정비 중이면 무시
    void resume();   // 재개(Working으로) — 고장/정비 중이면 무시

    // ---- 파생 클래스가 반드시 구현하는 "정체성" (순수 가상) ----------------
    // 새 기계 타입을 추가할 때 이 4개만 구현하면 됨(시뮬레이션 루프 수정 불필요 → OCP).
    virtual std::string typeName() const = 0;                    // 표시용 타입명
    virtual ProcessType processType() const = 0;                 // 공정 종류
    virtual MachineRole role() const = 0;                        // 처리 가능한 역할
    virtual bool canAcceptRecipe(const Recipe& recipe) const = 0; // 레시피 수용 여부

protected:
    // 파생 클래스가 상세 스펙(속도/초기HP/고장확률)을 지정해 생성하는 생성자.
    Machine(
        MachineId id,
        std::string name,
        double processingSpeed,
        double initialHealth,
        double breakdownProbability);

    // 이벤트 버스가 연결돼 있으면 Event 를 발행(없으면 무시). const 인 이유: 조회성
    // 동작 중에도 호출하기 위함(논리적 상태는 안 바뀜).
    void notify(EventType type, const std::string& message) const;

private:
    // 상태 객체들은 Machine 의 private 전이/진행 함수를 직접 호출한다 → friend.
    friend class IdleState;
    friend class WorkingState;
    friend class BrokenState;
    friend class MaintenanceState;

    // ---- 내부 상태 전이 도우미 (State 패턴의 전이 로직) --------------------
    void setState(std::unique_ptr<MachineState> state);  // exit→교체→enter
    void transitionToIdle(const std::string& reason);
    void transitionToWorking(const std::string& reason);
    void transitionToBroken(const std::string& reason);
    void transitionToMaintenance(const std::string& reason);
    void onStateTransition(MachineStatus from, MachineStatus to, const std::string& reason);
    void advanceProduction(double deltaTime);   // WorkingState 가 호출(생산/손상 처리)
    void advanceMaintenance(double deltaTime);  // MaintenanceState 가 호출(수리 진행)

    // ---- 데이터 멤버 (전부 private = 캡슐화) -------------------------------
    MachineId id_;                                  // 식별자
    std::string name_;                              // 이름
    std::optional<Recipe> recipe_;                  // 할당된 레시피(옵션)
    std::shared_ptr<ProductionTask> task_;          // 현재 작업(공유 소유, 0..1)
    std::unique_ptr<MachineState> state_;           // 현재 상태 객체(단독 소유) ★State패턴
    EventBus* eventBus_ = nullptr;                  // 이벤트 버스(비소유 참조)
    MachineStatus status_ = MachineStatus::Idle;    // 상태값(조회/가드용)
    double progress_ = 0.0;                         // 현재 단계 진행 누적(초)
    double health_ = 100.0;                         // HP
    double processingSpeed_ = 1.0;                  // 처리 속도 배수
    double breakdownProbability_ = 0.0;             // 고장 확률(현재 손상은 HP기반)
    double maintenanceElapsed_ = 0.0;               // 정비 경과 시간
    double maintenanceDuration_ = 2.0;              // 정비 소요 시간(생성자에서 설정)
    SimulationTime simulationTime_ = 0.0;           // 이벤트 타임스탬프용 누적 시간
};

// -----------------------------------------------------------------------------
// 구체 기계 4종. 공통 동작(상태/HP/진행)은 전부 base(Machine)가 처리하고,
// 각 스테이션은 "자기 역할/공정 종류/타입명/레시피 수용 여부"만 override 한다.
//
// 스펙은 data/factory_config.json 의 의도와 일치(생성자 기본 인자):
//   MixingStation    HP 150, 고장확률 0.02  - 가장 무거운 블렌딩 공정
//   QualityStation   HP 100, 고장확률 0.03  - 정밀 검사 + 탄산 주입
//   BottlingStation  HP 120, 고장확률 0.02  - 세척/충전/밀봉
//   PackagingStation HP 140, 고장확률 0.015 - 라벨+포장(마지막 단계)
// -----------------------------------------------------------------------------
class MixingStation final : public Machine {
public:
    MixingStation(
        MachineId id,
        std::string name,
        double processingSpeed = 1.0,
        double initialHealth = 150.0,
        double breakdownProbability = 0.02);

    std::string typeName() const override;
    ProcessType processType() const override;
    MachineRole role() const override;
    bool canAcceptRecipe(const Recipe& recipe) const override;
};

class QualityStation final : public Machine {
public:
    QualityStation(
        MachineId id,
        std::string name,
        double processingSpeed = 1.0,
        double initialHealth = 100.0,
        double breakdownProbability = 0.03);

    std::string typeName() const override;
    ProcessType processType() const override;
    MachineRole role() const override;
    bool canAcceptRecipe(const Recipe& recipe) const override;
};

class BottlingStation final : public Machine {
public:
    BottlingStation(
        MachineId id,
        std::string name,
        double processingSpeed = 1.0,
        double initialHealth = 120.0,
        double breakdownProbability = 0.02);

    std::string typeName() const override;
    ProcessType processType() const override;
    MachineRole role() const override;
    bool canAcceptRecipe(const Recipe& recipe) const override;
};

class PackagingStation final : public Machine {
public:
    PackagingStation(
        MachineId id,
        std::string name,
        double processingSpeed = 1.0,
        double initialHealth = 140.0,
        double breakdownProbability = 0.015);

    std::string typeName() const override;
    ProcessType processType() const override;
    MachineRole role() const override;
    bool canAcceptRecipe(const Recipe& recipe) const override;
};

} // namespace gactorio
