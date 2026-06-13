#pragma once

// =============================================================================
// SimulationHistory — Memento 패턴의 "Caretaker(관리인)"
// -----------------------------------------------------------------------------
// FactoryMemento 스냅샷들을 스택으로 보관한다. Controller 가 saveCheckpoint() 시
// push 하고, undo() 시 가장 최근 것을 pop 해 Factory(Originator)에게 돌려준다.
//
// Caretaker 규칙(중요):
//   - 메멘토의 내용을 절대 들여다보지 않는다(불투명 payload 로만 취급).
//   - 스냅샷이 어떻게 만들어지고 적용되는지 전혀 모른다.
// 즉 "저장/복원 방법"은 Factory 가, "보관"은 여기가 — 책임이 깔끔히 분리(SRP).
// 현재는 undo 스택만 있고 redo 스택은 없다.
// =============================================================================

#include "model/memento/FactoryMemento.hpp"

#include <optional>
#include <vector>

namespace gactorio {

class SimulationHistory {
public:
    void   push(FactoryMemento m);                // 스냅샷 적재
    bool   canUndo()   const;                     // 되돌릴 스냅샷이 있는가
    std::optional<FactoryMemento> pop();          // 최근 스냅샷 꺼내기(없으면 nullopt)
    void   clear();                               // 전체 비우기(공장 재생성 시)
    std::size_t size() const;                     // 보관 개수

private:
    std::vector<FactoryMemento> snapshots_;   // undo 스택(가장 뒤가 최신)
};

} // namespace gactorio
