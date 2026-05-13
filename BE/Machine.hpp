#pragma once

#include <string>

class Machine {
protected:
    std::string machineName;
    bool running;

public:
    Machine();
    Machine(const std::string& machineName);
    virtual ~Machine() = default;

    virtual void start();
    virtual void stop();
    virtual std::string getStatus() const;

    std::string getMachineName() const;
    bool isRunning() const;
};
