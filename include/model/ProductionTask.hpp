#pragma once

#include "model/Product.hpp"

#include <cstddef>
#include <memory>
#include <string>

namespace gactorio {

class ProductionTask {
public:
    explicit ProductionTask(const Product& product);
    explicit ProductionTask(std::shared_ptr<Product> product);

    const ProcessStep* currentStep() const;
    void advanceStep();
    bool isCompleted() const;
    double getProgressInRoute() const;
    ProductId getProductId() const;
    const std::string& getProductName() const;
    std::size_t currentStepIndex() const;
    std::size_t totalStepCount() const;

private:
    std::shared_ptr<Product> ownedProduct_;
    const Product* product_;
    std::size_t currentStepIndex_ = 0;
};

} // namespace gactorio
