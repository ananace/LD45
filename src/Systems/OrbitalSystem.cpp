#include "OrbitalSystem.hpp"
#include "../Util.hpp"

#include "../Components/GravityWell.hpp"
#include "../Components/Physical.hpp"
#include "../Components/CelestialBody.hpp"
#include "../Components/SatteliteBody.hpp"


#include <cmath>

using Systems::OrbitalSystem;
using namespace Components;

OrbitalSystem::OrbitalSystem()
{
}

OrbitalSystem::~OrbitalSystem()
{
}

void OrbitalSystem::update(const float aDt)
{
    auto& r = getRegistry();

    // r.view<Physical,GravityWell>()

    // Heavily simplified orbital mechanics
    r.view<SatteliteBody>().each([aDt, &r](auto& sattelite) {
        sf::Vector2f parentPosition;
        if (r.has<CelestialBody>(sattelite.Orbiting))
        {
            const auto& orbiting = r.get<const CelestialBody>(sattelite.Orbiting);
            parentPosition = orbiting.Position;
        }
        else
        {
            const auto& orbiting = r.get<const SatteliteBody>(sattelite.Orbiting);
            parentPosition = orbiting.CalculatedPosition;
        }

        sattelite.Angle += sattelite.Speed * aDt;

        sf::Vector2f direction(std::cos(sattelite.Angle), std::sin(sattelite.Angle));
        sattelite.CalculatedPosition = parentPosition + direction * sattelite.Distance;
    });

    // Ensure stellar modifications trickle down to sattelites
    orbitSetup();
}

void OrbitalSystem::onInit()
{
    auto& d = getDispatcher();

    d.sink<Events::OrbitRepair>().connect<&OrbitalSystem::onOrbitRepair>(*this);
}

void OrbitalSystem::onOrbitRepair(const Events::OrbitRepair& aEvent)
{
    orbitSetup(aEvent.Rounds);
}

void OrbitalSystem::orbitSetup(uint8_t aRounds)
{
    auto& r = getRegistry();

    for (; aRounds > 0; --aRounds)
    {
        r.view<SatteliteBody>().each([&r](auto& sattelite) {
            sf::Vector2f parentPosition;

            if (r.has<CelestialBody>(sattelite.Orbiting))
            {
                const auto& orbiting = r.get<const CelestialBody>(sattelite.Orbiting);
                parentPosition = orbiting.Position;
            }
            else
            {
                const auto& orbiting = r.get<const SatteliteBody>(sattelite.Orbiting);
                parentPosition = orbiting.CalculatedPosition;
            }

            sf::Vector2f direction(std::cos(sattelite.Angle), std::sin(sattelite.Angle));
            sattelite.CalculatedPosition = parentPosition + direction * sattelite.Distance;
        });
    }
}
