#include "dto/InventorySnapshot.hpp"

#include <utility>

// =============================================================================
// InventorySnapshot.cpp — 재고 DTO 구현(항목 = 문자열 ID + 수량).
// =============================================================================

namespace gactorio {

InventoryEntrySnapshot::InventoryEntrySnapshot(std::string id, int quantity)
    : InventoryEntrySnapshot(std::move(id), quantity, "", "", false, 0) {}

InventoryEntrySnapshot::InventoryEntrySnapshot(
    std::string id,
    int quantity,
    std::string displayName,
    std::string kind,
    bool restockable,
    int restockAmount)
    : id_(std::move(id)),
      displayName_(std::move(displayName)),
      kind_(std::move(kind)),
      quantity_(quantity),
      restockable_(restockable),
      restockAmount_(restockAmount) {}

const std::string& InventoryEntrySnapshot::id() const {
    return id_;
}

const std::string& InventoryEntrySnapshot::displayName() const {
    return displayName_;
}

const std::string& InventoryEntrySnapshot::kind() const {
    return kind_;
}

int InventoryEntrySnapshot::quantity() const {
    return quantity_;
}

bool InventoryEntrySnapshot::restockable() const {
    return restockable_;
}

int InventoryEntrySnapshot::restockAmount() const {
    return restockAmount_;
}

void InventorySnapshot::addItem(std::string id, int quantity) {
    items_.emplace_back(std::move(id), quantity);
}

void InventorySnapshot::addItem(
    std::string id,
    int quantity,
    std::string displayName,
    std::string kind,
    bool restockable,
    int restockAmount) {
    items_.emplace_back(
        std::move(id),
        quantity,
        std::move(displayName),
        std::move(kind),
        restockable,
        restockAmount);
}

const std::vector<InventoryEntrySnapshot>& InventorySnapshot::items() const {
    return items_;
}

} // namespace gactorio

