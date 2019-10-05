#pragma once

#include "../BaseSystem.hpp"

#include "../Events/OrbitRepair.hpp"

namespace Systems
{

class OrbitalSystem : public BaseSystem
{
public:
    OrbitalSystem();
    ~OrbitalSystem();

    virtual void update(const float aDt) override;

private:
    void onInit() override;

    void onOrbitRepair(const Events::OrbitRepair& aEvent);
    void orbitSetup(uint8_t aRounds = 1);
};

}
