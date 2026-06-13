#pragma once

// =============================================================================
// Memento 패턴 — 공장 상태의 "체크포인트(스냅샷)" 구조
// -----------------------------------------------------------------------------
// 역할 분담:
//   Originator = Factory          : createMemento()로 스냅샷 생성, restore로 복원
//   Memento    = FactoryMemento 등 : 상태값을 담는 불투명 payload(여기 파일)
//   Caretaker  = SimulationHistory : 스냅샷 스택을 보관(내용은 들여다보지 않음)
//
// 캡처 범위:
//   - 시뮬레이션 시간
//   - 재고(원자재 + 완제품)
//   - 라인별: 대기 제품 ID 목록 + 기계별 HP/상태
//   - (CarbonationFactory가) 다음 라인/기계 ID
//
// 일부러 복원하지 않는 것:
//   - 진행 중 ProductionTask (복원 시 기계는 Idle로, 큐가 자연히 다시 채움)
//   - 통계 카운터/이벤트 로그 (그건 "실제로 일어난 역사"라 되돌리지 않음)
//
// ★ 캡슐화(과제 핵심): 세 Memento 클래스 모두 데이터 멤버는 private(끝에 _)이고
//   읽기는 const accessor 로만 한다. 즉 public data member 가 없다. (예전 버전은
//   struct + public 필드였으나 채점 원칙에 맞춰 private + accessor 로 교정함.)
// =============================================================================

#include "common/Types.hpp"

#include <cstddef>
#include <map>
#include <utility>
#include <vector>

namespace gactorio {

// 기계 1대의 복원 데이터: ID, HP, 상태.
class MachineMemento {
public:
    MachineMemento(MachineId id, double health, MachineStatus status)
        : id_(id), health_(health), status_(status) {}

    MachineId id() const { return id_; }            // 기계 ID
    double health() const { return health_; }       // 저장된 HP
    MachineStatus status() const { return status_; } // 저장된 상태

private:
    MachineId     id_;
    double        health_;
    MachineStatus status_;
};

// 라인 1개의 복원 데이터: ID, 대기 제품 ID 목록, 기계 메멘토 목록.
class LineMemento {
public:
    LineMemento(
        LineId id,
        std::vector<ProductId> queueProductIds,
        std::vector<MachineMemento> machines)
        : id_(id),
          queueProductIds_(std::move(queueProductIds)),
          machines_(std::move(machines)) {}

    LineId id() const { return id_; }
    const std::vector<ProductId>& queueProductIds() const { return queueProductIds_; }
    const std::vector<MachineMemento>& machines() const { return machines_; }

private:
    LineId                       id_;
    std::vector<ProductId>       queueProductIds_;   // 큐에 있던 제품 ID들
    std::vector<MachineMemento>  machines_;          // 라인 기계들의 메멘토(composition)
};

// 공장 전체의 복원 데이터.
class FactoryMemento {
public:
    FactoryMemento(
        SimulationTime simulationTime,
        std::map<ItemType, int> items,
        std::map<ProductId, int> products)
        : simulationTime_(simulationTime),
          items_(std::move(items)),
          products_(std::move(products)) {}

    SimulationTime simulationTime() const { return simulationTime_; }
    const std::map<ItemType, int>& items() const { return items_; }
    const std::map<ProductId, int>& products() const { return products_; }
    const std::vector<LineMemento>& lines() const { return lines_; }
    LineId nextLineId() const { return nextLineId_; }
    MachineId nextMachineId() const { return nextMachineId_; }

    void addLine(LineMemento line) {                 // 라인 메멘토 추가(생성 시)
        lines_.push_back(std::move(line));
    }

    void setNextIds(LineId nextLineId, MachineId nextMachineId) {  // 다음 ID 저장
        nextLineId_ = nextLineId;
        nextMachineId_ = nextMachineId;
    }

private:
    SimulationTime                simulationTime_;   // 캡처 시점 시뮬레이션 시간
    std::map<ItemType, int>       items_;            // 원자재 재고 스냅샷
    std::map<ProductId, int>      products_;         // 완제품 재고 스냅샷
    std::vector<LineMemento>      lines_;            // 라인 메멘토들(composition)
    LineId                        nextLineId_ = 0;   // 다음 라인 ID(0 = 미설정)
    MachineId                     nextMachineId_ = 0; // 다음 기계 ID(0 = 미설정)
};

} // namespace gactorio
