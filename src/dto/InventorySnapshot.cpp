#include "dto/InventorySnapshot.hpp"

#include <utility>

namespace gactorio {

InventoryEntrySnapshot::InventoryEntrySnapshot(std::string id, std::string name, int quantity)
    : id_(std::move(id)), name_(std::move(name)), quantity_(quantity) {}

const std::string& InventoryEntrySnapshot::id() const {
    return id_;
}

const std::string& InventoryEntrySnapshot::name() const {
    return name_;
}

int InventoryEntrySnapshot::quantity() const {
    return quantity_;
}

void InventorySnapshot::addItem(std::string id, std::string name, int quantity) {
    items_.emplace_back(std::move(id), std::move(name), quantity);
}

const std::vector<InventoryEntrySnapshot>& InventorySnapshot::items() const {
    return items_;
}

} // namespace gactorio
