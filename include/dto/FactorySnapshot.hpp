#pragma once

// =============================================================================
// FactorySnapshot — 공장 전체 상태의 읽기 전용 DTO (최상위 스냅샷)
// -----------------------------------------------------------------------------
// 시뮬레이션 시간 + 재고 + 통계 + 라인 스냅샷 목록 + 이벤트 스냅샷 목록을 묶는다.
// FactoryController::snapshot() 이 Model 을 순회하며 값을 복사해 만든다.
// composition: FactorySnapshot ◆ {Inventory/Statistics/Line/Event}Snapshot.
// =============================================================================

#include "dto/EventSnapshot.hpp"
#include "dto/InventorySnapshot.hpp"
#include "dto/ProductionLineSnapshot.hpp"
#include "dto/StatisticsSnapshot.hpp"
#include "common/Types.hpp"

#include <vector>

namespace gactorio {

class FactorySnapshot {
public:
    FactorySnapshot(SimulationTime simulationTime, InventorySnapshot inventory, StatisticsSnapshot statistics);

    SimulationTime simulationTime() const;             // 시뮬레이션 시간
    SimulationTime timeSeconds() const;                // (동의어)
    const InventorySnapshot& inventory() const;        // 재고 스냅샷
    const StatisticsSnapshot& statistics() const;      // 통계 스냅샷

    void addProductionLine(ProductionLineSnapshot line);  // 라인 스냅샷 추가
    void addEvent(EventSnapshot event);                   // 이벤트 스냅샷 추가

    const std::vector<ProductionLineSnapshot>& productionLines() const;
    const std::vector<EventSnapshot>& events() const;

private:
    SimulationTime simulationTime_;
    InventorySnapshot inventory_;
    StatisticsSnapshot statistics_;
    std::vector<ProductionLineSnapshot> productionLines_;   // 라인 스냅샷들
    std::vector<EventSnapshot> events_;                     // 이벤트 스냅샷들
};

} // namespace gactorio
