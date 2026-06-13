#include "dto/MachineSnapshot.hpp"

#include <utility>

// =============================================================================
// MachineSnapshot.cpp — 생성자에서 값들을 복사 저장하고 접근자로 읽기만 제공.
// 로직 없는 순수 DTO(불변 값 객체).
// =============================================================================

namespace gactorio {

MachineSnapshot::MachineSnapshot(
    MachineId id,
    std::string name,
    std::string typeName,
    MachineStatus status,
    std::string stateName,
    double progress,
    double health)
    : id_(id),
      name_(std::move(name)),
      typeName_(std::move(typeName)),
      status_(status),
      stateName_(std::move(stateName)),
      progress_(progress),
      health_(health) {}

MachineId MachineSnapshot::id() const {
    return id_;
}

const std::string& MachineSnapshot::name() const {
    return name_;
}

const std::string& MachineSnapshot::typeName() const {
    return typeName_;
}

MachineStatus MachineSnapshot::status() const {
    return status_;
}

const std::string& MachineSnapshot::stateName() const {
    return stateName_;
}

double MachineSnapshot::progress() const {
    return progress_;
}

double MachineSnapshot::health() const {
    return health_;
}

} // namespace gactorio
