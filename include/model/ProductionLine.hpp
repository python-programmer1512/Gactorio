#pragma once

// =============================================================================
// ProductionLine — 한 생산라인: 작업 큐 + 순서가 있는 기계 묶음
// -----------------------------------------------------------------------------
// 책임:
//   * 기계들을 unique_ptr<Machine> 로 "소유"한다(composition). 다형 컨테이너라서
//     라인은 어떤 구체 기계가 들어있는지 몰라도 된다.
//   * 대기 작업 큐(taskQueue_)를 관리하고, 비어 있는 적합한 기계에 작업을 배정한다
//     (assignAvailableTask). 배정은 역할(role) 일치로만 판단 → 타입 분기 없음.
//   * 완료된 제품 ID를 모아(collectCompletedProducts) Factory 에 넘긴다.
//
// 복사 금지/이동 허용: 기계를 unique_ptr 로 소유하므로 복사는 막고(=delete),
// vector<ProductionLine> 안에서 재배치될 수 있도록 이동은 허용한다.
// =============================================================================

#include "common/Types.hpp"
#include "model/Machine.hpp"
#include "model/Product.hpp"
#include "model/ProductionTask.hpp"
#include "model/events/EventBus.hpp"

#include <deque>
#include <memory>
#include <string>
#include <vector>

namespace gactorio {

class ProductionLine {
public:
    ProductionLine(ProductionLineId id, std::string name);
    ProductionLine(const ProductionLine&) = delete;             // 복사 금지(unique_ptr 소유)
    ProductionLine& operator=(const ProductionLine&) = delete;
    ProductionLine(ProductionLine&&) noexcept = default;        // 이동 허용
    ProductionLine& operator=(ProductionLine&&) noexcept = default;

    ProductionLineId id() const;
    const std::string& name() const;
    const std::vector<std::unique_ptr<Machine>>& machines() const;  // 소유 기계 목록

    void setEventBus(EventBus* eventBus);                  // 버스 연결(기계들에도 전파)
    void enqueueProduct(std::shared_ptr<Product> product); // 제품을 작업으로 큐에 추가
    std::size_t queueLength() const;                       // 대기 작업 수
    std::shared_ptr<ProductionTask> currentTask() const;   // 큐 맨 앞 작업
    void assignAvailableTask();                            // 유휴 기계에 적합 작업 배정
    std::vector<ProductId> collectCompletedProducts();     // 완료 제품 ID 수거
    void addMachine(std::unique_ptr<Machine> machine);     // 기계 추가(소유 이전)
    Machine* findMachine(MachineId id);                    // ID로 기계 찾기
    const Machine* findMachine(MachineId id) const;
    void update(double deltaTime);                         // 라인 단독 갱신(테스트용 경로)

    // ---- Memento 지원 (Factory 의 생성/복원에서 사용) ----------------------
    std::vector<ProductId> pendingProductIds() const;  // 큐에 남은 제품 ID 목록
    void clearQueue();                                 // 큐 비우기
    void clearCompleted();                             // 완료 목록 비우기

private:
    ProductionLineId id_;                                       // 라인 ID
    std::string name_;                                          // 라인 이름
    std::deque<std::shared_ptr<ProductionTask>> taskQueue_;     // 대기 작업 큐(공유 소유)
    std::vector<ProductId> completedProducts_;                  // 이번 틱 완료 제품 임시
    std::vector<std::unique_ptr<Machine>> machines_;            // 소유 기계들(composition)
    EventBus* eventBus_ = nullptr;                              // 이벤트 버스(비소유)
};

} // namespace gactorio
