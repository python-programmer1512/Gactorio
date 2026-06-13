#include "model/Machine.hpp"

#include "common/Config.h"
#include "model/MachineStates.hpp"

#include <algorithm>
#include <random>
#include <utility>

// =============================================================================
// Machine.cpp — Machine 의 동작 구현
// 핵심: (1) State 패턴 전이 로직, (2) 매 틱 생산 진행 + 무작위 HP 손상,
//       (3) 정비/수리 처리, (4) 이벤트 발행. 데이터는 전부 Machine 의 private 멤버.
// =============================================================================

namespace gactorio {

namespace {
// HP 손상 확률 판정을 위한 스레드 로컬 난수 생성기.
// 매 틱 reseed 하지 않고 한 번만 시드해 성능을 아끼면서, 실행마다 적당히 무작위.
std::mt19937& rng() {
    thread_local std::mt19937 gen{std::random_device{}()};
    return gen;
}
// [0,1) 균등 분포 난수 한 개.
double uniform01() {
    std::uniform_real_distribution<double> d(0.0, 1.0);
    return d(rng());
}
} // namespace

// 간단 생성자: 기본 스펙(속도 1, HP 100, 고장확률 0)으로 위임 생성.
Machine::Machine(MachineId id, std::string name)
    : Machine(id, std::move(name), 1.0, 100.0, 0.0) {}

// 상세 생성자: 스펙을 받아 초기화. 초기 상태는 IdleState.
Machine::Machine(
    MachineId id,
    std::string name,
    double processingSpeed,
    double initialHealth,
    double breakdownProbability)
    : id_(id),
      name_(std::move(name)),
      state_(std::make_unique<IdleState>()),                                  // 시작은 Idle
      health_(std::clamp(initialHealth, 0.0, config::kInitialHealth)),        // HP 클램프
      processingSpeed_(std::max(0.0, processingSpeed)),                       // 음수 방지
      breakdownProbability_(std::clamp(breakdownProbability, 0.0, 1.0)) {     // 0~1 클램프
    maintenanceDuration_ = config::kRepairAllDelaySeconds;   // 정비 소요 시간 설정
    if (health_ <= 0.0) {                                    // HP 0으로 생성되면 즉시 고장 상태
        status_ = MachineStatus::Broken;
        state_ = std::make_unique<BrokenState>();
    }
}

Machine::~Machine() = default;

MachineId Machine::id() const {
    return id_;
}

const std::string& Machine::name() const {
    return name_;
}

double Machine::progress() const {
    return getProgress();
}

// 현재 상태 객체의 이름. 상태 객체가 없으면 "Unknown".
std::string Machine::stateName() const {
    return state_ ? state_->name() : "Unknown";
}

const std::optional<Recipe>& Machine::recipe() const {
    return recipe_;
}

// 작업을 들고 있고 아직 끝나지 않았으면 true.
bool Machine::hasTask() const {
    return task_ != nullptr && !task_->isCompleted();
}

const ProductionTask* Machine::currentTask() const {
    return hasTask() ? task_.get() : nullptr;
}

// 작업 배정 시도. 수락 불가/역할 불일치면 false. 성공 시 이벤트 발행 + Working 전이.
bool Machine::assignTask(std::shared_ptr<ProductionTask> task) {
    if (!canAcceptTask() || task == nullptr || task->currentStep() == nullptr) {
        return false;
    }
    if (!canProcess(task->currentStep()->requiredRole())) {   // 이 기계 역할로 처리 가능한 단계인가
        return false;
    }

    task_ = std::move(task);
    progress_ = 0.0;
    notify(EventType::TaskEnqueued, name_ + " accepted " + task_->getProductName());
    notify(EventType::TaskStarted, name_ + " started " + task_->getProductName());
    transitionToWorking("task assigned");
    return true;
}

// 레시피는 이 기계가 수용 가능할 때만 설정.
void Machine::setRecipe(const Recipe& recipe) {
    if (canAcceptRecipe(recipe)) {
        recipe_ = recipe;
    }
}

// 상태 교체: 이전 상태 exit → 새 상태로 교체 → 새 상태 enter (State 패턴의 전이 절차).
void Machine::setState(std::unique_ptr<MachineState> state) {
    if (state_) {
        state_->exit(*this);
    }
    state_ = std::move(state);
    if (state_) {
        state_->enter(*this);
    }
}

void Machine::setEventBus(EventBus* eventBus) {
    eventBus_ = eventBus;
}

// 매 틱 호출: 이벤트 타임스탬프용 시간 누적 후, 현재 상태에 행동을 위임(State 패턴 핵심).
void Machine::update(double deltaTime) {
    simulationTime_ += std::max(0.0, deltaTime);
    if (state_) {
        state_->update(*this, deltaTime);
    }
}

// 작업 수락 가능 조건: 작업 없음 + Idle + HP > 0.
bool Machine::canAcceptTask() const {
    return task_ == nullptr && status_ == MachineStatus::Idle && health_ > 0.0;
}

// 이 기계 역할이 주어진 역할과 일치하는가(타입 분기 대신 역할 비교).
bool Machine::canProcess(MachineRole role) const {
    return this->role() == role;
}

// 강제 고장(디버그/테스트 훅). 작업을 버리므로 수리 후 깨끗이 Idle 로 복귀한다.
// (게임 흐름상 HP 소진 고장은 이 경로를 안 거치고 작업을 보존 → Repair 시 처음부터 재개.)
void Machine::forceBreak() {
    task_.reset();
    transitionToBroken("forced breakdown");
}

// 수리 요청: 정비(Maintenance) 상태로 전이 → 시간이 차면 HP 전량 회복.
void Machine::repair() {
    transitionToMaintenance("repair requested");
}

// Memento 복원용: 작업 폐기, 진행률/정비타이머 0, HP 덮어쓰기, 주어진 상태로 강제 전이.
void Machine::resetForRestore(double newHealth, MachineStatus newStatus) {
    task_.reset();
    progress_           = 0.0;
    maintenanceElapsed_ = 0.0;
    health_             = std::clamp(newHealth, 0.0, config::kInitialHealth);

    switch (newStatus) {
    case MachineStatus::Broken:
        status_ = MachineStatus::Broken;
        setState(std::make_unique<BrokenState>());
        break;
    case MachineStatus::Maintenance:
        status_ = MachineStatus::Maintenance;
        setState(std::make_unique<MaintenanceState>());
        break;
    case MachineStatus::Working:   // 진행 중 작업은 복원 안 하므로 Working/Idle 둘 다 Idle 로
    case MachineStatus::Idle:
    default:
        status_ = MachineStatus::Idle;
        setState(std::make_unique<IdleState>());
        break;
    }
}

// 소량 HP 회복(+5). 상태는 안 바꾸고, 고장 상태는 되살리지 않음.
void Machine::incrementalRepair() {
    if (status_ == MachineStatus::Broken) {
        return;
    }
    health_ = std::min(100.0, health_ + config::kIncrementalRepairHp);
}

MachineId Machine::getId() const {
    return id_;
}

const std::string& Machine::getName() const {
    return name_;
}

MachineStatus Machine::getStatus() const {
    return status_;
}

// 진행률(0~1)을 "경로 전체" 기준으로 계산:
//   작업이 없으면 progress_ 그대로. 작업이 있으면 (이미 끝난 단계 비율) +
//   (현재 단계 내부 진행률 / 전체 단계 수). UI 의 진행 막대에 쓰인다.
double Machine::getProgress() const {
    if (task_ == nullptr || task_->currentStep() == nullptr) {
        return progress_;
    }

    const auto totalSteps = task_->totalStepCount();
    if (totalSteps == 0) {
        return 1.0;
    }

    const auto currentStepProgress = progress_ / task_->currentStep()->baseDurationSeconds();
    return task_->getProgressInRoute() + (currentStepProgress / static_cast<double>(totalSteps));
}

double Machine::getHealth() const {
    return health_;
}

double Machine::getProcessingSpeed() const {
    return processingSpeed_;
}

double Machine::getBreakdownProbability() const {
    return breakdownProbability_;
}

bool Machine::breakdownsEnabled() const {
    return breakdownsEnabled_;
}

void Machine::setBreakdownsEnabled(bool enabled) {
    breakdownsEnabled_ = enabled;
}

// 일시정지: 고장/정비 중이면 무시. 그 외엔 Idle 로 전이하고 MachinePaused 이벤트 발행.
void Machine::pause() {
    if (status_ == MachineStatus::Broken || status_ == MachineStatus::Maintenance) {
        return;
    }
    transitionToIdle("paused");
    notify(EventType::MachinePaused, name_ + " paused");
}

// 재개: 고장/정비 중이면 무시. 그 외엔 Working 으로 전이.
void Machine::resume() {
    if (status_ == MachineStatus::Broken || status_ == MachineStatus::Maintenance) {
        return;
    }
    transitionToWorking("resumed");
    notify(EventType::TaskStarted, name_ + " resumed");
}

void Machine::instantRepair() {
    health_ = config::kInitialHealth;
    maintenanceElapsed_ = 0.0;
    progress_ = 0.0;
    notify(EventType::MachineRepaired, name_ + " instantly repaired");
    if (breakdownsEnabled_ && task_ != nullptr) {
        transitionToWorking("instant repair, resuming task");
    } else {
        transitionToIdle("instant repair completed");
    }
}

// WorkingState 가 매 틱 호출. 생산 진행과 무작위 손상을 처리한다.
void Machine::advanceProduction(double deltaTime) {
    // ----- 무작위 마모/손상 ------------------------------------------------
    // 작업을 들고 가동 중일 때, 매 틱 작은 확률로 HP가 깎인다. 확률/손상 범위는
    // data/factory_config.json → config:: 상수. HP가 0이 되면 작업은 "보존"되어
    // Repair-All 이 처음부터 다시 진행할 수 있게 한다.
    if (task_ != nullptr) {
        const double dt = std::max(0.0, deltaTime);
        const double damageChance = config::kDamageChancePerSecond * dt;
        if (uniform01() < damageChance) {
            const double range = config::kDamageMaxHp - config::kDamageMinHp;
            const double dmg   = config::kDamageMinHp + uniform01() * range;
            health_ = std::max(0.0, health_ - dmg);
            if (health_ <= 0.0) {
                progress_ = 0.0;
                transitionToBroken("HP depleted while working");  // 고장 전이(작업은 보존)
                return;
            }
        }
    }
    // ----------------------------------------------------------------------

    // 작업이 있고 현재 단계가 있으면: 그 단계를 시간만큼 진행.
    if (task_ != nullptr && task_->currentStep() != nullptr) {
        const auto stepDuration = task_->currentStep()->baseDurationSeconds();
        if (stepDuration <= 0.0) {   // 0초 단계는 즉시 완료 처리
            task_->advanceStep();
            notify(EventType::StepCompleted, name_ + " completed a process step");
            progress_ = 0.0;
            return;
        }

        progress_ += std::max(0.0, deltaTime) * processingSpeed_;   // 진행 누적(속도 반영)
        if (progress_ >= stepDuration) {                            // 단계 완료
            progress_ = 0.0;
            task_->advanceStep();
            notify(EventType::StepCompleted, name_ + " completed a process step");
            if (task_->isCompleted()) {                             // 마지막 단계였다면 제품 완성
                notify(EventType::ProductCompleted, name_ + " completed " + task_->getProductName());
                task_.reset();
                transitionToIdle("task completed");
            } else {                                                // 아직 단계 남음: 다음 기계가 받도록 작업 놓기
                task_.reset();
                transitionToIdle("step completed");
            }
        }
        return;
    }

    // 작업은 없지만 레시피가 설정된 경우(보조 경로): 레시피 시간 기준으로 진행.
    if (!recipe_.has_value() || recipe_->durationSeconds() <= 0.0) {
        return;
    }

    progress_ += (std::max(0.0, deltaTime) * processingSpeed_) / recipe_->durationSeconds();
    if (progress_ >= 1.0) {
        progress_ = 0.0;
        notify(EventType::ProductCompleted, name_ + " completed " + recipe_->name());
    }
}

// MaintenanceState 가 매 틱 호출. 정비 시간이 차면 HP 회복 후 적절한 상태로 복귀.
void Machine::advanceMaintenance(double deltaTime) {
    maintenanceElapsed_ += std::max(0.0, deltaTime);
    if (maintenanceElapsed_ >= maintenanceDuration_) {
        health_ = config::kInitialHealth;   // HP 전량 회복
        progress_ = 0.0;
        notify(EventType::MachineRepaired, name_ + " repaired");
        if (task_ != nullptr) {
            // 고장 시 보존된 작업이 있으면 현재 단계 처음부터 재개.
            notify(EventType::TaskStarted, name_ + " restarted " + task_->getProductName());
            transitionToWorking("repair complete, resuming task");
        } else {
            // 작업이 없으면(강제고장/정상완료) Idle 로 복귀.
            transitionToIdle("maintenance completed");
        }
    }
}

// ---- 상태 전이 함수들: status_ 갱신 + 상태 객체 교체 + 전이 이벤트 발행 ------
void Machine::transitionToIdle(const std::string& reason) {
    const auto previous = status_;
    status_ = MachineStatus::Idle;
    maintenanceElapsed_ = 0.0;
    setState(std::make_unique<IdleState>());
    onStateTransition(previous, status_, reason);
}

void Machine::transitionToWorking(const std::string& reason) {
    // 고장(Broken)에서는 막는다(반드시 수리 먼저). 단 정비(Maintenance)에서는 허용 —
    // 정비 완료가 보존된 작업을 재개하는 통로이기 때문.
    if (status_ == MachineStatus::Broken) {
        return;
    }

    const auto previous = status_;
    status_ = MachineStatus::Working;
    setState(std::make_unique<WorkingState>());
    onStateTransition(previous, status_, reason);
}

void Machine::transitionToBroken(const std::string& reason) {
    const auto previous = status_;
    health_ = 0.0;       // 고장 = HP 0
    progress_ = 0.0;
    status_ = MachineStatus::Broken;
    setState(std::make_unique<BrokenState>());
    onStateTransition(previous, status_, reason);
    notify(EventType::MachineBroken, name_ + " broke down");
}

void Machine::transitionToMaintenance(const std::string& reason) {
    const auto previous = status_;
    maintenanceElapsed_ = 0.0;
    progress_ = 0.0;
    status_ = MachineStatus::Maintenance;
    setState(std::make_unique<MaintenanceState>());
    onStateTransition(previous, status_, reason);
}

// 상태가 실제로 바뀌었을 때만 StateChanged 이벤트 발행(같은 상태로의 전이는 무시).
void Machine::onStateTransition(MachineStatus from, MachineStatus to, const std::string& reason) {
    if (from == to) {
        return;
    }

    notify(EventType::StateChanged, name_ + " state changed: " + reason);
}

// 이벤트 발행: 버스가 연결돼 있으면 Event 를 publish. (Observer 패턴의 발행 지점)
void Machine::notify(EventType type, const std::string& message) const {
    if (eventBus_ != nullptr) {
        eventBus_->publish(Event(simulationTime_, type, id_, message));
    }
}

// -----------------------------------------------------------------------------
// 구체 스테이션 4종. base Machine 이 상태/HP/진행을 처리하므로, 각 스테이션은
// "자기 정체성(타입명/공정종류/역할)"과 "레시피 수용 여부"만 선언한다.
// canAcceptRecipe 는 현재 전부 true (역할 매칭은 작업 배정 단계에서 별도 판단).
// -----------------------------------------------------------------------------
MixingStation::MixingStation(
    MachineId id,
    std::string name,
    double processingSpeed,
    double initialHealth,
    double breakdownProbability)
    : Machine(id, std::move(name), processingSpeed, initialHealth, breakdownProbability) {}

std::string MixingStation::typeName() const                      { return "Mixing Station"; }
ProcessType MixingStation::processType() const                   { return ProcessType::Mixing; }
MachineRole MixingStation::role() const                          { return MachineRole::Mixing; }
bool MixingStation::canAcceptRecipe(const Recipe& recipe) const  { (void)recipe; return true; }

QualityStation::QualityStation(
    MachineId id,
    std::string name,
    double processingSpeed,
    double initialHealth,
    double breakdownProbability)
    : Machine(id, std::move(name), processingSpeed, initialHealth, breakdownProbability) {}

std::string QualityStation::typeName() const                     { return "Quality Station"; }
ProcessType QualityStation::processType() const                  { return ProcessType::Quality; }
MachineRole QualityStation::role() const                         { return MachineRole::Quality; }
bool QualityStation::canAcceptRecipe(const Recipe& recipe) const { (void)recipe; return true; }

BottlingStation::BottlingStation(
    MachineId id,
    std::string name,
    double processingSpeed,
    double initialHealth,
    double breakdownProbability)
    : Machine(id, std::move(name), processingSpeed, initialHealth, breakdownProbability) {}

std::string BottlingStation::typeName() const                     { return "Bottling Station"; }
ProcessType BottlingStation::processType() const                  { return ProcessType::Bottling; }
MachineRole BottlingStation::role() const                         { return MachineRole::Bottling; }
bool BottlingStation::canAcceptRecipe(const Recipe& recipe) const { (void)recipe; return true; }

PackagingStation::PackagingStation(
    MachineId id,
    std::string name,
    double processingSpeed,
    double initialHealth,
    double breakdownProbability)
    : Machine(id, std::move(name), processingSpeed, initialHealth, breakdownProbability) {}

std::string PackagingStation::typeName() const                     { return "Packaging Station"; }
ProcessType PackagingStation::processType() const                  { return ProcessType::Packaging; }
MachineRole PackagingStation::role() const                         { return MachineRole::Packaging; }
bool PackagingStation::canAcceptRecipe(const Recipe& recipe) const { (void)recipe; return true; }

} // namespace gactorio
