#pragma once

#include "../BaseSystem.hpp"

namespace Systems
{

class InputSystem : public BaseSystem
{
public:
    InputSystem();
    ~InputSystem();

    virtual void update(const float aDt) override;

private:
    void onInit() override;
};

}
