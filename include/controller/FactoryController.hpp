#pragma once

// =============================================================================
// gactorio::FactoryController — 백엔드 "유스케이스" 컨트롤러
// -----------------------------------------------------------------------------
// CarbonationFactory(Model)를 소유하고, UI 가 필요로 하는 명령/조회를 안정적인
// 공개 메서드로 노출한다. 또 Memento 의 Caretaker(SimulationHistory)를 들고
// save/undo 파사드를 제공한다.
//
// 위치(2단 컨트롤러):
//   ctrl::Controller (View-facing, ctrl:: 타입만 노출)
//        └─ 내부에서 이 gactorio::FactoryController 를 호출(번역은 Controller.cpp)
//   gactorio::FactoryController (이 파일, gactorio:: Model 타입 사용)
//
// 이 컨트롤러는 Model 상태를 DTO 스냅샷(FactorySnapshot 등)으로 변환해 반환하며,
// 가변 Model 객체를 외부로 절대 노출하지 않는다(캡슐화).
// =============================================================================

#include "common/Types.hpp"
#include "controller/FactoryCommand.hpp"
#include "controller/SimulationHistory.hpp"
#include "dto/EventSnapshot.hpp"
#include "dto/FactorySnapshot.hpp"
#include "dto/StatisticsSnapshot.hpp"
#include "model/CarbonationFactory.hpp"

#include <cstddef>
#include <memory>
#include <vector>

namespace gactorio {

class FactoryController {
public:
    FactoryController();   // 생성 시 기본 CarbonationFactory 구성

    void createDefaultCarbonationFactory();   // 새 공장 생성 + 히스토리 초기화
    void reset();                             // = 새 공장 재생성
    void tick(double deltaTime);              // 시뮬레이션 한 틱 진행
    void startSimulation();                   // = resume
    void pauseSimulation();
    void resetSimulation();
    void resumeSimulation();
    void resetSimulationClock();
    void stopSimulation();
    void setSpeed(double speedMultiplier);
    void setSimulationSpeed(double speedMultiplier);
    FactoryCommandResult enqueueProduct(LineId lineId, ProductType productType);
    FactoryCommandResult enqueueProductById(LineId lineId, ProductId productId);
    // 큐가 가장 짧은 라인에 enqueue. 선택된 LineId 반환(아무도 못 받으면 0).
    LineId               enqueueAuto(ProductType productType);
    LineId               enqueueAutoById(ProductId productId);
    // 새 음료 라인 추가 후 LineId 반환.
    LineId               addLine();
    // 유휴 라인(빈 큐 + 진행 작업 없음)일 때만 제거.
    FactoryCommandResult removeLine(LineId id);
    FactoryCommandResult forceBreak(MachineId id);
    FactoryCommandResult repairMachine(MachineId id);             // 정비(지연 후 전량 회복)
    FactoryCommandResult instantRepairMachine(MachineId id);
    FactoryCommandResult incrementalRepairMachine(MachineId id);  // +5 HP 즉시
    FactoryCommandResult restockItem(ItemType itemType, int amount);
    void setRandomBreakdownsEnabled(bool enabled);
    bool randomBreakdownsEnabled() const;
    void clearEventLog();
    FactoryCommandResult pauseMachine(MachineId id);
    FactoryCommandResult resumeMachine(MachineId id);

    // ---- 조회: Model 상태를 읽기 전용 DTO 로 변환 -------------------------
    FactorySnapshot getFactorySnapshot() const;
    std::vector<EventSnapshot> getEventLogs() const;
    StatisticsSnapshot getStatistics() const;
    FactorySnapshot snapshot() const;

    // ---- Memento (Caretaker 파사드) ---------------------------------------
    void        saveCheckpoint();      // factory_->createMemento() 를 history_ 에 push
    bool        undo();                // history_ 에서 pop 해 factory_ 복원
    bool        canUndo() const;
    std::size_t historySize() const;

private:
    std::unique_ptr<CarbonationFactory> factory_;   // 활성 공장(단독 소유, Originator)
    SimulationHistory                   history_;   // 스냅샷 스택(Caretaker)
};

} // namespace gactorio
