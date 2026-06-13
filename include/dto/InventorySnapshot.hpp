#pragma once

// =============================================================================
// InventorySnapshot — 재고 DTO (항목 = InventoryEntrySnapshot 들의 목록)
// -----------------------------------------------------------------------------
// 항목 ID 는 문자열로 보관한다(원자재는 "1".."5", 완제품은 "101".."103").
// 상위 컨트롤러(ctrl::Controller)가 이 문자열 ID를 다시 사람이 읽는 이름과
// "원자재/제품 여부"로 변환해 View 로 넘긴다.
// =============================================================================

#include <string>
#include <vector>

namespace gactorio {

// 재고 한 항목: (문자열 ID, 수량).
class InventoryEntrySnapshot {
public:
    InventoryEntrySnapshot(std::string id, int quantity);

    const std::string& id() const;   // 문자열 ID(ItemType 또는 ProductId의 숫자)
    int quantity() const;            // 수량

private:
    std::string id_;
    int quantity_;
};

class InventorySnapshot {
public:
    void addItem(std::string id, int quantity);                    // 항목 추가
    const std::vector<InventoryEntrySnapshot>& items() const;      // 항목 목록

private:
    std::vector<InventoryEntrySnapshot> items_;   // 재고 항목들(composition)
};

} // namespace gactorio
