#pragma once

#include <string>
#include <vector>

namespace gactorio {

class InventoryEntrySnapshot {
public:
    InventoryEntrySnapshot(std::string id, std::string name, int quantity);

    const std::string& id() const;
    const std::string& name() const;
    int quantity() const;

private:
    std::string id_;
    std::string name_;
    int quantity_;
};

class InventorySnapshot {
public:
    void addItem(std::string id, std::string name, int quantity);
    const std::vector<InventoryEntrySnapshot>& items() const;

private:
    std::vector<InventoryEntrySnapshot> items_;
};

} // namespace gactorio
