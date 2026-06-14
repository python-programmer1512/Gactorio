#pragma once

// =============================================================================
// CarbonationFactory — the project's concrete Factory (is-a Factory, ▷).
//
// UML references (BE_Overall_Class_Diagram, Memento_Pattern_Diagram):
//   * Overrides createMemento()/restoreFromMemento() (Memento Originator) and
//     the createProductById()/createLineForMemento() hooks so snapshots can be
//     rebuilt from ProductId/recipe data.
//   * Seeds one 4-station beverage line and can spawn more via addDynamicLine().
// =============================================================================

#include "model/Factory.hpp"
#include "model/Recipe.hpp"

#include <vector>

namespace gactorio {

class CarbonationFactory final : public Factory {
public:
    CarbonationFactory();

    const std::vector<Recipe>& recipes() const;

    // Spawn a new 4-station beverage line and append it to the factory.
    // Returns the LineId of the freshly added line.
    LineId addDynamicLine();

    FactoryMemento createMemento() const override;
    void restoreFromMemento(const FactoryMemento& memento) override;

protected:
    // Memento helper — maps a ProductId back to a ProductCatalog entry.
    std::shared_ptr<Product> createProductById(ProductId id) const override;
    std::optional<ProductionLine> createLineForMemento(const LineMemento& memento) const override;

private:
    std::vector<Recipe> recipes_;
    LineId    nextLineId_    = 2;   // 1 is the seeded default line
    MachineId nextMachineId_ = 5;   // 1..4 are the seeded line's machines
};

} // namespace gactorio

