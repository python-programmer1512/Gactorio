#include "dto/InventorySnapshot.hpp"

#include <utility>

namespace gactorio {

InventoryEntrySnapshot::InventoryEntrySnapshot(std::string id, int quantity)
    : id_(std::move(id)), quantity_(quantity) {}

const std::string& InventoryEntrySnapshot::id() const {
    return id_;
}

int InventoryEntrySnapshot::quantity() const {
    return quantity_;
}

void InventorySnapshot::addItem(std::string id, int quantity) {
    items_.emplace_back(std::move(id), quantity);
}

const std::vector<InventoryEntrySnapshot>& InventorySnapshot::items() const {
    return items_;
}

} // namespace gactorio

