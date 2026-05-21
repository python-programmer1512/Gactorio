#include "../../include/model/Factory.hpp"

namespace factory {

Factory::Factory()
    : factoryName("Unnamed Factory") {
}

Factory::Factory(const std::string& factoryName)
    : factoryName(factoryName) {
}

std::string Factory::getFactoryName() const {
    return factoryName;
}

}