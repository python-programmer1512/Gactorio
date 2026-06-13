#pragma once

// =============================================================================
// ProductionLineSnapshot — 생산라인 한 개의 상태 DTO (기계 스냅샷들을 포함)
// -----------------------------------------------------------------------------
// 라인 ID/이름/대기 수/현재 작업명·진행률 + 소속 기계 스냅샷 목록을 담는다.
// composition: ProductionLineSnapshot ◆ MachineSnapshot.
// =============================================================================

#include "common/Types.hpp"
#include "dto/MachineSnapshot.hpp"

#include <cstddef>
#include <string>
#include <vector>

namespace gactorio {

class ProductionLineSnapshot {
public:
    ProductionLineSnapshot(
        ProductionLineId id,
        std::string name,
        std::size_t queueLength = 0,
        std::string currentTaskName = "",
        double currentTaskProgress = 0.0);

    ProductionLineId id() const;
    const std::string& name() const;
    std::size_t queueLength() const;                  // 대기 작업 수
    const std::string& currentTaskName() const;       // 현재 작업 제품명(없으면 빈 문자열)
    double currentTaskProgress() const;               // 현재 작업 진행률
    void setCurrentTaskProgress(double progress);     // 진행률 보정(컨트롤러가 설정)
    void addMachine(MachineSnapshot machine);         // 기계 스냅샷 추가
    const std::vector<MachineSnapshot>& machines() const;

private:
    ProductionLineId id_;
    std::string name_;
    std::size_t queueLength_;
    std::string currentTaskName_;
    double currentTaskProgress_;
    std::vector<MachineSnapshot> machines_;   // 소속 기계 스냅샷들(composition)
};

using LineSnapshot = ProductionLineSnapshot;   // 가독성용 별칭

} // namespace gactorio
