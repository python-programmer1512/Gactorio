#pragma once

#include <string>
#include <vector>

namespace gactorio {

class InventoryEntrySnapshot {
public:
    InventoryEntrySnapshot(std::string id, int quantity);

    const std::string& id() const;
    int quantity() const;

private:
    std::string id_;
    int quantity_;
};

class InventorySnapshot {
public:
    void addItem(std::string id, int quantity);
    const std::vector<InventoryEntrySnapshot>& items() const;

private:
    std::vector<InventoryEntrySnapshot> items_;
};

} // namespace gactorio

