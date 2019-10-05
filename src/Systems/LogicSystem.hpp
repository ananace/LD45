#pragma once

#include "../BaseSystem.hpp"

namespace Systems
{

class LogicSystem : public BaseSystem
{
public:
    LogicSystem();
    ~LogicSystem();

    virtual void update(const float aDt) override;

private:
    void onInit() override;
};

}
