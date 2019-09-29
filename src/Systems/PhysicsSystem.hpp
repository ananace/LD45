#pragma once

#include "../BaseSystem.hpp"

namespace Systems
{

class PhysicsSystem : public BaseSystem
{
public:
    PhysicsSystem();
    ~PhysicsSystem();

    virtual void update(const float aDt);
};

}
