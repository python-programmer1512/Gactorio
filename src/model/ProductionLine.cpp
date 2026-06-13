#include "model/ProductionLine.hpp"

#include <algorithm>
#include <utility>

// =============================================================================
// ProductionLine.cpp — 라인의 작업 큐 운영과 기계 배정 구현
// 핵심: assignAvailableTask() 는 "유휴 기계 × 큐의 작업" 을 역할(role)로만 매칭한다.
//       구체 기계 타입을 절대 보지 않는다(다형성/OCP 유지).
// =============================================================================

namespace gactorio {

namespace {

// 어떤 작업이 이미 (이 라인의) 어떤 기계에 배정돼 있는지 검사. 같은 작업이 두 기계에
// 동시에 배정되는 것을 막기 위함.
bool isAssignedToMachine(
    const std::vector<std::unique_ptr<Machine>>& machines,
    const ProductionTask* task) {
    return std::any_of(
        machines.begin(),
        machines.end(),
        [task](const std::unique_ptr<Machine>& machine) {
            return machine->currentTask() == task;
        });
}

} // namespace

ProductionLine::ProductionLine(ProductionLineId id, std::string name)
    : id_(id), name_(std::move(name)) {}

ProductionLineId ProductionLine::id() const {
    return id_;
}

const std::string& ProductionLine::name() const {
    return name_;
}

const std::vector<std::unique_ptr<Machine>>& ProductionLine::machines() const {
    return machines_;
}

// 버스 연결을 라인 자신과 소속 기계 전부에 전파.
void ProductionLine::setEventBus(EventBus* eventBus) {
    eventBus_ = eventBus;
    for (auto& machine : machines_) {
        machine->setEventBus(eventBus_);
    }
}

// 제품을 작업(ProductionTask)으로 감싸 큐 뒤에 추가. TaskEnqueued 이벤트 발행.
void ProductionLine::enqueueProduct(std::shared_ptr<Product> product) {
    if (product == nullptr) {
        return;
    }

    auto task = std::make_shared<ProductionTask>(std::move(product));
    if (eventBus_ != nullptr) {
        eventBus_->publish(Event(0.0, EventType::TaskEnqueued, 0, name_ + " enqueued " + task->getProductName()));
    }
    taskQueue_.push_back(std::move(task));
}

std::size_t ProductionLine::queueLength() const {
    return taskQueue_.size();
}

std::shared_ptr<ProductionTask> ProductionLine::currentTask() const {
    if (taskQueue_.empty()) {
        return nullptr;
    }
    return taskQueue_.front();   // 큐 맨 앞(가장 먼저 들어온 작업)
}

// 기계 추가: 버스 연결 후 소유 이전.
void ProductionLine::addMachine(std::unique_ptr<Machine> machine) {
    machine->setEventBus(eventBus_);
    machines_.push_back(std::move(machine));
}

// 유휴 기계마다, 큐의 작업 중 "현재 단계 역할이 이 기계 역할과 맞고 아직 미배정"인
// 첫 작업을 찾아 배정한다. 타입 분기 없이 역할(role)만으로 매칭(다형성).
void ProductionLine::assignAvailableTask() {
    if (taskQueue_.empty()) {
        return;
    }

    for (auto& machine : machines_) {
        if (!machine->canAcceptTask()) {
            continue;   // 유휴+HP>0+작업없음 이 아니면 건너뜀
        }

        for (const auto& task : taskQueue_) {
            if (task == nullptr || task->isCompleted()) {
                continue;
            }
            if (isAssignedToMachine(machines_, task.get())) {
                continue;   // 이미 다른 기계가 처리 중
            }

            const auto* step = task->currentStep();
            if (step != nullptr && machine->canProcess(step->requiredRole())) {
                machine->assignTask(task);   // 역할 일치 → 배정
                break;                        // 이 기계는 한 작업만
            }
        }
    }
}

// 완료된 작업들을 큐에서 빼서 그 제품 ID 목록을 반환(Factory 가 재고에 더함).
std::vector<ProductId> ProductionLine::collectCompletedProducts() {
    for (auto it = taskQueue_.begin(); it != taskQueue_.end();) {
        if (*it != nullptr && (*it)->isCompleted()) {
            completedProducts_.push_back((*it)->getProductId());
            it = taskQueue_.erase(it);
        } else {
            ++it;
        }
    }

    auto completed = std::move(completedProducts_);
    completedProducts_.clear();
    return completed;
}

Machine* ProductionLine::findMachine(MachineId id) {
    for (auto& machine : machines_) {
        if (machine->getId() == id) {
            return machine.get();
        }
    }
    return nullptr;
}

const Machine* ProductionLine::findMachine(MachineId id) const {
    for (const auto& machine : machines_) {
        if (machine->getId() == id) {
            return machine.get();
        }
    }
    return nullptr;
}

// 라인 단독 갱신(테스트/단독 사용 경로). 실제 런타임은 Factory::update 가 단계를 조율한다.
void ProductionLine::update(double deltaTime) {
    assignAvailableTask();
    for (auto& machine : machines_) {
        machine->update(deltaTime);
    }
    (void)collectCompletedProducts();
    assignAvailableTask();
}

// ---- Memento 지원 ----------------------------------------------------------
// 큐에 남은 작업들의 제품 ID 목록(복원 시 다시 enqueue 하기 위함).
std::vector<ProductId> ProductionLine::pendingProductIds() const {
    std::vector<ProductId> out;
    out.reserve(taskQueue_.size());
    for (const auto& task : taskQueue_) {
        if (task != nullptr) out.push_back(task->getProductId());
    }
    return out;
}

void ProductionLine::clearQueue() {
    taskQueue_.clear();
}

void ProductionLine::clearCompleted() {
    completedProducts_.clear();
}

} // namespace gactorio
