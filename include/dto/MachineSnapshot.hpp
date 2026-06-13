#pragma once

// =============================================================================
// MachineSnapshot — 기계 한 대의 상태를 담은 읽기 전용 DTO
// -----------------------------------------------------------------------------
// FactoryController 가 Model(Machine)에서 값을 복사해 만든다. UI/상위 계층은 이
// 스냅샷만 보고 렌더링하며, 절대 Machine 포인터를 들고 있지 않는다(캡슐화 경계).
// =============================================================================

#include "common/Types.hpp"

#include <string>

namespace gactorio {

class MachineSnapshot {
public:
    MachineSnapshot(
        MachineId id,
        std::string name,
        std::string typeName,
        MachineStatus status,
        std::string stateName,
        double progress,
        double health);

    MachineId id() const;                    // 기계 ID
    const std::string& name() const;         // 기계 이름
    const std::string& typeName() const;     // 타입명("Mixing Station" 등)
    MachineStatus status() const;            // 상태 enum
    const std::string& stateName() const;    // 상태 이름 문자열
    double progress() const;                 // 진행률 0~1
    double health() const;                   // HP

private:
    MachineId id_;
    std::string name_;
    std::string typeName_;
    MachineStatus status_;
    std::string stateName_;
    double progress_;
    double health_;
};

} // namespace gactorio
