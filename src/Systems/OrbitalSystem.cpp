#include "OrbitalSystem.hpp"
#include "../Util.hpp"

#include "../Components/Tags/InSystem.hpp"
#include "../Components/Tags/SystemCore.hpp"
#include "../Components/GravityWell.hpp"
#include "../Components/Position.hpp"
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
    r.group<SatteliteBody>(entt::get<Position>).each([aDt, &r](auto& sattelite, auto& position) {
        const auto& orbiting = r.get<const Position>(sattelite.Orbiting);
        const auto& parentPosition = orbiting.Position;

        sattelite.CurrentAngle += sattelite.Speed * aDt;

        sf::Vector2f direction(std::cos(sattelite.CurrentAngle), std::sin(sattelite.CurrentAngle));
        position.Position = parentPosition + direction * sattelite.Distance;
    });

    // Ensure stellar modifications trickle down to sattelites
    orbitSetup();

    // Ensure far objects are detached from their system
    systemExclusion();

    // Ensure nearby objects are attached to the system
    systemInclusion();
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
        r.group<SatteliteBody>(entt::get<Position>).each([&r](auto& sattelite, auto& position) {
            const auto& orbiting = r.get<const Position>(sattelite.Orbiting);
            const auto& parentPosition = orbiting.Position;

            sf::Vector2f direction(std::cos(sattelite.CurrentAngle), std::sin(sattelite.CurrentAngle));
            position.Position = parentPosition + direction * sattelite.Distance;
        });
    }
}

void OrbitalSystem::systemExclusion()
{
    auto& r = getRegistry();

    r.group<const Components::Tags::SystemCore>(entt::get<const Components::Position>).each([&r](auto systemEnt, const auto& core, const auto& body) {
        r.group<const Components::Tags::InSystem>(entt::get<const Components::Position>).each([systemEnt, &r, &body](auto ent, const auto& sys, const auto& position) {
            if (sys.System != systemEnt)
                return;

            auto distance = Util::GetLength(position.Position - body.Position);

            if (distance >= kSystemSize)
                r.remove<Components::Tags::InSystem>(ent);
        });
    });
}

void OrbitalSystem::systemInclusion()
{
    auto& r = getRegistry();

    r.group<const Components::Tags::SystemCore>(entt::get<const Components::Position>).each([&r](auto systemEnt, const auto& core, const auto& body) {
        r.group<>(entt::get<const Components::Position>, entt::exclude<Components::Tags::InSystem>).each([systemEnt, &r, &body](auto ent, const auto& position) {
            auto distance = Util::GetLength(position.Position - body.Position);

            if (distance < kSystemSize)
                r.assign<Components::Tags::InSystem>(ent, systemEnt);
        });
    });
}
