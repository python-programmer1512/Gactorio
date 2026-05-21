#include "../../include/model/Machine.hpp"

namespace factory {

Machine::Machine()
    : machineName("Unnamed Machine"),
      running(false) {
}

Machine::Machine(const std::string& machineName)
    : machineName(machineName),
      running(false) {
}

void Machine::start() {
    running = true;
}

void Machine::stop() {
    running = false;
}

std::string Machine::getStatus() const {
    if (running) {
        return machineName + " is running.";
    }

    return machineName + " is stopped.";
}

std::string Machine::getMachineName() const {
    return machineName;
}

bool Machine::isRunning() const {
    return running;
}

}