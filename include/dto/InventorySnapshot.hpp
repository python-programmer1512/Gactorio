#pragma once

#include <string>
#include <vector>

namespace gactorio {

class InventoryEntrySnapshot {
public:
    InventoryEntrySnapshot(std::string id, int quantity);
    InventoryEntrySnapshot(
        std::string id,
        int quantity,
        std::string displayName,
        std::string kind,
        bool restockable,
        int restockAmount);

    const std::string& id() const;
    const std::string& displayName() const;
    const std::string& kind() const;
    int quantity() const;
    bool restockable() const;
    int restockAmount() const;

private:
    std::string id_;
    std::string displayName_;
    std::string kind_;
    int quantity_;
    bool restockable_;
    int restockAmount_;
};

class InventorySnapshot {
public:
    void addItem(std::string id, int quantity);
    void addItem(
        std::string id,
        int quantity,
        std::string displayName,
        std::string kind,
        bool restockable,
        int restockAmount);
    const std::vector<InventoryEntrySnapshot>& items() const;

private:
    std::vector<InventoryEntrySnapshot> items_;
};

} // namespace gactorio

