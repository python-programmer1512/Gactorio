#pragma once

#include "Machine.h"

#include <memory>
#include <vector>

// Factory는 4개의 Machine을 composition으로 보유. 각 머신은 polymorphic하게
// unique_ptr<Machine>으로 저장되어 실제 타입은 MixingStation/QualityStation/...
class Factory {
public:
    Factory();

    std::size_t machineCount() const { return m_machines.size(); }

    // View가 읽기만 할 때 사용
    const Machine& machineAt(std::size_t i) const { return *m_machines[i]; }

    // Controller가 상태 변경할 때 사용
    Machine& machineAt(std::size_t i) { return *m_machines[i]; }

private:
    std::vector<std::unique_ptr<Machine>> m_machines;
};
