#include "OrbitalSystem.hpp"
#include "../Util.hpp"

#include "../Components/Tags/InSystem.hpp"
#include "../Components/Tags/SystemCore.hpp"
#include "../Components/GravityWell.hpp"
#include "../Components/Physical.hpp"
#include "../Components/CelestialBody.hpp"
#include "../Components/SatteliteBody.hpp"

#include "entt/entity/utility.hpp"

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

    // Ensure far objects are detached from their system
    systemExclusion();

    // Ensure nearby objects are attached to the system
    systemInclusion();

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

void OrbitalSystem::systemExclusion()
{
    auto& r = getRegistry();

    r.group<const Components::Tags::SystemCore, const Components::CelestialBody>().each([&r](auto systemEnt, const auto& core, const auto& body) {
        r.group<const Components::Tags::InSystem, const Components::Physical>().each([systemEnt, &r, &body](auto ent, const auto& sys, const auto& physical) {
            if (sys.System != systemEnt)
                return;

            auto distance = Util::GetLength(physical.Position - body.Position);

            if (distance >= 10000)
                r.remove<Components::Tags::InSystem>(ent);
        });
    });
}

void OrbitalSystem::systemInclusion()
{
    auto& r = getRegistry();

    r.group<const Components::Tags::SystemCore, const Components::CelestialBody>().each([&r](auto systemEnt, const auto& core, const auto& body) {
        r.group<const Components::Physical>(entt::exclude<Components::Tags::InSystem>).each([systemEnt, &r, &body](auto ent, const auto& physical) {
            auto distance = Util::GetLength(physical.Position - body.Position);

            if (distance < 10000)
                r.assign<Components::Tags::InSystem>(ent, systemEnt);
        });
    });
}
