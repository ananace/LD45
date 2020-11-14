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
    auto sattelites = r.group<SatteliteBody>(entt::get<Position>);
    for (auto satteliteEnt : sattelites)
    {
        auto& sattelite = sattelites.get<SatteliteBody>(satteliteEnt);
        auto& position = sattelites.get<Position>(satteliteEnt);
        const auto& orbiting = sattelites.get<Position>(sattelite.Orbiting);
        const auto& parentPosition = orbiting.Position;

        sattelite.CurrentAngle += sattelite.Speed * aDt;

        sf::Vector2f direction(std::cos(sattelite.CurrentAngle), std::sin(sattelite.CurrentAngle));
        position.Position = parentPosition + direction * sattelite.Distance;
    };

    // Ensure stellar modifications trickle down to sattelites
    orbitSetup();

    // Ensure objects are attached and detached from their system
    systemHandling();
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
            const auto& orbiting = r.get<Position>(sattelite.Orbiting);
            const auto& parentPosition = orbiting.Position;

            sf::Vector2f direction(std::cos(sattelite.CurrentAngle), std::sin(sattelite.CurrentAngle));
            position.Position = parentPosition + direction * sattelite.Distance;
        });
    }
}

void OrbitalSystem::systemHandling()
{
    auto& r = getRegistry();

    auto systems = r.group<Components::Tags::SystemCore>(entt::get<Components::Position>);
    auto insystem = r.group<Components::Tags::InSystem>(entt::get<Components::Position>);
    auto nonsystem = r.group<>(entt::get<Components::Position>, entt::exclude<Components::Tags::InSystem>);

    for (auto systemEnt : systems)
    {
        for (auto insystemEnt : insystem)
        {
            auto& sys = insystem.get<Components::Tags::InSystem>(insystemEnt);
            if (sys.System != systemEnt)
                continue;

            auto& position = insystem.get<Components::Position>(insystemEnt);
            auto& body = systems.get<Components::Position>(systemEnt);
            auto distance = Util::GetLength(position.Position - body.Position);

            if (distance >= kSystemSize)
            {
                printf("[OrbitSystem|D] Removing entity %d from system %d\n", int(r.entity(insystemEnt)), int(r.entity(systemEnt)));
                r.remove<Components::Tags::InSystem>(insystemEnt);
            }
        };

        for (auto nonsystemEnt : nonsystem)
        {
            auto& body = systems.get<Components::Position>(systemEnt);
            auto& position = nonsystem.get<Components::Position>(nonsystemEnt);

            auto distance = Util::GetLength(position.Position - body.Position);

            if (distance < kSystemSize)
            {
                printf("[OrbitSystem|D] Adding entity %d to system %d\n", int(r.entity(nonsystemEnt)), int(r.entity(systemEnt)));
                r.emplace<Components::Tags::InSystem>(nonsystemEnt, systemEnt);
            }
        };
    };
}
