#pragma once

// =============================================================================
// Factory — 시뮬레이션 최상위 집합체(aggregate root) + Memento 의 Originator
// -----------------------------------------------------------------------------
// 소유(composition): 생산라인들, 전역 재고, 시뮬레이션 시계, 이벤트 버스, 그리고
//   두 Observer(EventLog/Statistics). 생성자에서 두 Observer 를 버스에 구독시킨다.
//
// 핵심 동작 update():
//   시계 갱신 → (선)작업배정 → machines_ 캐시를 돌며 Machine::update() 다형 호출 →
//   완료 제품 수거해 재고 반영 → (후)작업배정. 이 루프 어디에도 구체 기계 타입 분기가
//   없다(과제 요구). machines_ 는 라인들 안의 기계를 가리키는 비소유 raw 포인터 캐시다.
//
// Memento(Originator): createMemento()로 현재 상태를 불투명 스냅샷으로 캡처하고,
//   restoreFromMemento()로 복원한다. 제품/라인 재구성은 가상 훅으로 분리해 파생
//   Factory(CarbonationFactory)가 구체 토폴로지를 채우게 한다(Template Method 성격).
//
// 추상화/DIP: Factory 는 Machine/Observer 같은 추상에 의존하고 구체 기계 타입은 모른다.
// =============================================================================

#include "common/SimClock.hpp"
#include "common/Types.hpp"
#include "model/Inventory.hpp"
#include "model/ProductionLine.hpp"
#include "model/events/EventBus.hpp"
#include "model/events/EventLogObserver.hpp"
#include "model/events/StatisticsObserver.hpp"
#include "model/memento/FactoryMemento.hpp"

#include <memory>
#include <optional>
#include <vector>

namespace gactorio {

class Factory {
public:
    Factory();
    virtual ~Factory() = default;   // CarbonationFactory 가 상속하므로 가상 소멸자

    // ---- 조회 접근자 -------------------------------------------------------
    SimulationTime simulationTime() const;                       // 현재 시뮬레이션 시간
    const Inventory& inventory() const;
    Inventory& inventory();                                      // (파생/내부 설정용) 가변 재고
    const std::vector<ProductionLine>& productionLines() const;  // 라인 목록
    const std::vector<Machine*>& machines() const;               // 기계 포인터 캐시
    const EventLog& eventLog() const;                            // 이벤트 로그 Observer
    const Statistics& statistics() const;                        // 통계 Observer
    EventBus& eventBus();
    const EventBus& eventBus() const;
    const SimClock& clock() const;

    // ---- 명령 -------------------------------------------------------------
    void addProductionLine(ProductionLine line);                 // 라인 추가(버스 연결 + 캐시 갱신)
    bool removeProductionLine(LineId id);                        // 라인 제거(유휴일 때만, 최소 1개 유지)
    bool enqueueProduct(LineId lineId, std::shared_ptr<Product> product); // 재료 소비 후 큐 등록
    bool restockItem(ItemType itemType, int amount);            // 원자재 보충
    ProductionLine* findProductionLine(LineId id);
    const ProductionLine* findProductionLine(LineId id) const;
    Machine* findMachine(MachineId id);
    SimulationTime update(double realDeltaTime);                // ★ 매 틱 시뮬레이션 진행
    void pauseClock();
    void resumeClock();
    void resetClock();
    void stopClock();
    void setClockSpeed(double speedMultiplier);

    // ---- Memento (Originator 측) ------------------------------------------
    // 현재 공장 상태를 불투명 스냅샷으로 캡처. virtual: 파생이 추가 상태(다음 ID 등)를 덧붙임.
    virtual FactoryMemento createMemento() const;
    // 캡처된 스냅샷으로 복원. 진행 중 작업은 복원하지 않고(기계는 Idle+이전 HP로 리셋),
    // 라인 큐는 스냅샷의 제품 ID로 다시 채워 작업이 자연스레 재개되게 한다.
    virtual void   restoreFromMemento(const FactoryMemento& memento);

protected:
    EventLog& mutableEventLog();        // 파생/내부에서 로그 가변 접근
    Statistics& mutableStatistics();    // 파생/내부에서 통계 가변 접근

    // 파생이 제공하는 "ID→Product 생성" 훅. base Factory 는 카탈로그를 모르므로 가상.
    // CarbonationFactory 가 ProductCatalog 로 위임 구현한다.
    virtual std::shared_ptr<Product> createProductById(ProductId id) const;
    // 파생이 제공하는 "LineMemento→ProductionLine 재구성" 훅(구체 스테이션 구성 담당).
    virtual std::optional<ProductionLine> createLineForMemento(const LineMemento& memento) const;

private:
    void rebuildMachineCache();   // 라인들로부터 machines_ 포인터 캐시 재구성

    SimClock clock_;                          // 시뮬레이션 시계(composition)
    Inventory inventory_;                     // 전역 재고(composition)
    std::vector<ProductionLine> productionLines_;  // 라인들(composition)
    std::vector<Machine*> machines_;          // 라인 내부 기계를 가리키는 비소유 캐시
    EventBus eventBus_;                       // 이벤트 발행 버스(composition)
    EventLogObserver eventLog_;               // 로그 Observer(composition)
    StatisticsObserver statistics_;           // 통계 Observer(composition)
};

} // namespace gactorio
