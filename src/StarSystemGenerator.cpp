#include "StarSystemGenerator.hpp"
#include "Math.hpp"
#include "Util.hpp"

#include "Components/Tags/SystemCore.hpp"
#include "Components/Tags/TracedOrbit.hpp"
#include "Components/Atmosphere.hpp"
#include "Components/CelestialBody.hpp"
#include "Components/Colony.hpp"
#include "Components/GateShape.hpp"
#include "Components/JumpConnection.hpp"
#include "Components/PlanetShape.hpp"
#include "Components/Position.hpp"
#include "Components/Renderables.hpp"
#include "Components/SatteliteBody.hpp"
#include "Components/StarField.hpp"
#include "Components/StarShape.hpp"

#include <entt/entity/registry.hpp>

#include <random>
#include <vector>

void StarSystemGenerator::createSystem(entt::registry& aReg, const sf::Vector2f& aCenter)
{
    std::uniform_real_distribution<float> randRadDist(0.f, Math::PI * 2.f);
    std::uniform_real_distribution<float> starSizeDist(90.f, 200.f);
    std::uniform_real_distribution<float> planetSizeDist(4.f, 50.f);
    std::uniform_real_distribution<float> moonSizeDist(3.f, 8.f);
    std::uniform_real_distribution<float> stellarSpaceDist(1.75f, 8.f);
    std::uniform_real_distribution<float> atmosphereThicknessDist(1.0f, 2.5f);
    std::uniform_int_distribution<uint8_t> starTypeDist(Components::StarShape::O, Components::StarShape::M);
    std::uniform_int_distribution<int> reverseOrbitChanceDist(0, 14);
    std::uniform_int_distribution<int> binarySystemChanceDist(0, 9);
    std::uniform_int_distribution<int> moonChanceDist(0, 3);
    std::uniform_int_distribution<int> atmosphereChanceDist(0, 4);
    std::uniform_int_distribution<uint8_t> planetCountDist(0, 10);
    std::uniform_int_distribution<uint8_t> moonCountDist(1, 6);
    std::uniform_int_distribution<uint8_t> colorDist(0, 255);

    // 2.5m - 30m for a full orbit
    std::uniform_real_distribution<float> orbitPeriodDist(2.5f * 60.f, 30.f * 60.f);

    std::random_device rDev;

    bool binarySystem = binarySystemChanceDist(rDev) == 0;

    auto systemCenter = aReg.create();
    aReg.assign<Components::Position>(systemCenter, aCenter);
    aReg.assign<Components::Tags::SystemCore>(systemCenter);

    float currentDistance = 0;

    if (binarySystem)
    {
        printf("[SSG|D] Generating Binary system\n");
        auto starType1 = Components::StarShape::StarType(starTypeDist(rDev));
        auto starSize1 = starSizeDist(rDev);
        auto starType2 = Components::StarShape::StarType(starTypeDist(rDev));
        auto starSize2 = starSizeDist(rDev);

        float orbitTime = ((Math::PI * 2.f) / orbitPeriodDist(rDev));

        float ang = randRadDist(rDev);

        // Orbit points, in the centerpoint between system center and the star
        auto star1Orbit = std::get<0>(aReg.create<Components::Position>());
        aReg.assign<Components::SatteliteBody>(star1Orbit, systemCenter, ((starSize1 + starSize2) / 2) * 1.5f, orbitTime, ang);

        auto star2Orbit = std::get<0>(aReg.create<Components::Position>());
        aReg.assign<Components::SatteliteBody>(star2Orbit, systemCenter, ((starSize1 + starSize2) / 2) * 1.5f, orbitTime, ang + Math::PI);

        auto star1 = std::get<0>(aReg.create<Components::CelestialBody, Components::Position, Components::Renderable>());
        aReg.assign<Components::SatteliteBody>(star1, star1Orbit, ((starSize1 + starSize2) / 2), orbitTime, ang + Math::PI * 1.5f);
        aReg.assign<Components::StarShape>(star1, starSize1, starType1);

        auto star2 = std::get<0>(aReg.create<Components::CelestialBody, Components::Position, Components::Renderable>());
        aReg.assign<Components::SatteliteBody>(star2, star2Orbit, ((starSize1 + starSize2) / 2), -orbitTime, ang - Math::PI * 1.5f);
        aReg.assign<Components::StarShape>(star2, starSize2, starType2);

        currentDistance += ((starSize1 + starSize2) / 2) * stellarSpaceDist(rDev);
    }
    else
    {
        printf("[SSG|D] Generating system\n");
        auto starType = Components::StarShape::StarType(starTypeDist(rDev));

        aReg.assign<Components::CelestialBody>(systemCenter);
        auto& shape = aReg.assign<Components::StarShape>(systemCenter, starSizeDist(rDev), starType);
        auto& rend = aReg.assign<Components::Renderable>(systemCenter);
        rend.Position = rend.LastPosition = aCenter;

        currentDistance += shape.Size * stellarSpaceDist(rDev);
    }

    auto planets = planetCountDist(rDev);
    printf("[SSG|D] Generating %d planets:\n", planets);

    for (; planets > 0; --planets)
    {
        const auto planet = std::get<0>(aReg.create<Components::Position, Components::Renderable, Components::Tags::TracedOrbit>());
        const float planetSize = planetSizeDist(rDev);
        printf("[SSG|D] - Generating planet of size %.1f\n", planetSize);

        const auto& shape = aReg.assign<Components::PlanetShape>(planet, planetSize, sf::Color(colorDist(rDev), colorDist(rDev), colorDist(rDev)));

        const bool hasAtmosphere = atmosphereChanceDist(rDev) == 0;
        if (hasAtmosphere)
        {
            printf("[SSG|D]   with atmosphere\n");
            const auto& atmos = aReg.assign<Components::Atmosphere>(planet, planetSize, planetSize * atmosphereThicknessDist(rDev), shape.Color);

            currentDistance += atmos.OuterSize * 0.75f;
        }
        else
            currentDistance += planetSize * 0.75f;

        const bool hasMoons = moonChanceDist(rDev) == 0;
        float totalMoonRadius{};
        if (hasMoons)
        {
            totalMoonRadius += planetSize;

            const auto moons = moonCountDist(rDev);
            printf("[SSG|D]   with %d moons:\n", moons);
            for (uint8_t i = 0; i < moons; ++i)
            {
                float moonSize = moonSizeDist(rDev);
                auto moon = std::get<0>(aReg.create<Components::Position, Components::Renderable, Components::Tags::TracedOrbit>());
                auto& shape = aReg.assign<Components::PlanetShape>(moon, moonSize, sf::Color(colorDist(rDev), colorDist(rDev), colorDist(rDev)));
                printf("[SSG|D]   - Generating moon of size %.1f\n", moonSize);

                float additionalRad = 0.f;
                const bool hasAtmosphere = atmosphereChanceDist(rDev) == 0;
                if (hasAtmosphere)
                {
                    printf("[SSG|D]     with atmosphere\n");
                    const auto& atmos = aReg.assign<Components::Atmosphere>(moon, moonSize, moonSize * atmosphereThicknessDist(rDev), shape.Color);

                    additionalRad += atmos.OuterSize * 0.75f;
                }

                const bool moonHasMoon = moonChanceDist(rDev) == 0 && moonChanceDist(rDev) == 0;
                if (moonHasMoon)
                {
                    printf("[SSG|D]     with a moon of its own:\n");
                    float moonMoonSize = moonSizeDist(rDev);
                    auto moonMoon = std::get<0>(aReg.create<Components::Position, Components::Renderable, Components::Tags::TracedOrbit>());
                    auto& shape = aReg.assign<Components::PlanetShape>(moonMoon, moonMoonSize, sf::Color(colorDist(rDev), colorDist(rDev), colorDist(rDev)));

                    printf("[SSG|D]     - Generating moon of size %.1f\n", moonMoonSize);

                    const bool hasAtmosphere = atmosphereChanceDist(rDev) == 0;
                    if (hasAtmosphere)
                    {
                        printf("[SSG|D]       with atmosphere\n");
                        const auto& atmos = aReg.assign<Components::Atmosphere>(moonMoon, moonMoonSize, moonMoonSize * atmosphereThicknessDist(rDev), shape.Color);

                        additionalRad += atmos.OuterSize * 0.75f;
                    }

                    float orbitDir = (reverseOrbitChanceDist(rDev) == 0 ? -1 : 1);
                    const auto& body = aReg.assign<Components::SatteliteBody>(moonMoon, moon, moonSize + moonMoonSize * stellarSpaceDist(rDev), ((Math::PI * 2.f) / orbitPeriodDist(rDev)) * orbitDir, randRadDist(rDev));
                    printf("[SSG|D]       at an orbit of %.2f\n", body.Distance);
                    additionalRad += body.Distance;
                }

                totalMoonRadius += additionalRad + moonSize * stellarSpaceDist(rDev);

                float orbitDir = (reverseOrbitChanceDist(rDev) == 0 ? -1 : 1);
                auto& body = aReg.assign<Components::SatteliteBody>(moon, planet, totalMoonRadius, ((Math::PI * 2.f) / orbitPeriodDist(rDev)) * orbitDir, randRadDist(rDev));
                printf("[SSG|D]     with an orbital distance of %.2f:\n", body.Distance);
            }
        }

        float orbitDir = (reverseOrbitChanceDist(rDev) == 0 ? -1 : 1);
        auto& body = aReg.assign<Components::SatteliteBody>(planet, systemCenter, currentDistance + totalMoonRadius * 1.75f, ((Math::PI * 2.f) / orbitPeriodDist(rDev)) * orbitDir, randRadDist(rDev));
        printf("[SSG|D]   with orbit distance of %.2f\n", body.Distance);
        printf("[SSG|D]   and orbit period of %.2fs\n\n", (Math::PI * 2.f) / body.Speed);

        currentDistance += totalMoonRadius * 3.5f + planetSize * stellarSpaceDist(rDev);
    }

    printf("[SSG|D] Final system size %f\n", currentDistance);

    currentDistance = std::max(currentDistance, 5000.f);

    auto& starfield = aReg.assign<Components::StarField>(systemCenter);
    starfield.Count = 2048;
    starfield.FieldSize.left = aCenter.x - currentDistance * 2.5f;
    starfield.FieldSize.top = aCenter.y - currentDistance * 2.5f;
    starfield.FieldSize.width = currentDistance * 5.f;
    starfield.FieldSize.height = currentDistance * 5.f;
}
void StarSystemGenerator::createJumpGates(entt::registry& aReg)
{
}
void StarSystemGenerator::createJumpHoles(entt::registry& aReg)
{
    auto systems = aReg.view<Components::Tags::SystemCore, Components::Position>();
    std::vector<entt::entity> systemEnts;
    systemEnts.reserve(systems.size());
    for (auto& ent : systems)
        systemEnts.push_back(ent);

    std::uniform_int_distribution<size_t> systemDist(0, systemEnts.size() - 1);
    std::uniform_real_distribution<float> satteliteOffsetDist(-(Math::PI / 8.f), +(Math::PI / 8.f));
    std::random_device rDev;

    const auto gates = int(systems.size() * 0.75f);
    for (int i = 0; i < gates; ++i)
    {
        auto sysEnt1 = systemEnts[systemDist(rDev)];
        auto sysEnt2 = sysEnt1;
        while (sysEnt1 == sysEnt2)
            sysEnt2 = systemEnts[systemDist(rDev)];

        printf("[SSG|D] Adding jumpgate between systems %d and %d\n", int(aReg.entity(sysEnt1)), int(aReg.entity(sysEnt2)));

        auto& sysPos1 = systems.get<Components::Position>(sysEnt1);
        auto& sysPos2 = systems.get<Components::Position>(sysEnt2);

        auto gate1 = std::get<0>(aReg.create<Components::Renderable, Components::Position>());
        auto gate2 = std::get<0>(aReg.create<Components::Renderable, Components::Position>());

        auto dir1to2 = Util::GetAngle(Util::GetNormalized(sysPos2.Position - sysPos1.Position));
        auto dir2to1 = Util::GetAngle(Util::GetNormalized(sysPos1.Position - sysPos2.Position));

        aReg.assign<Components::SatteliteBody>(gate1, sysEnt1, 2500.f, 0.f, dir1to2 + satteliteOffsetDist(rDev));
        aReg.assign<Components::GateShape>(gate1, dir1to2);
        aReg.assign<Components::JumpConnection>(gate1, gate2, 10.f);

        aReg.assign<Components::SatteliteBody>(gate2, sysEnt2, 2500.f, 0.f, dir2to1 + satteliteOffsetDist(rDev));
        aReg.assign<Components::GateShape>(gate2, dir2to1);
        aReg.assign<Components::JumpConnection>(gate2, gate1, 10.f);

        printf("[SSG|D] Added jumpgate\n");
    }
}

void StarSystemGenerator::createColonies(entt::registry& aReg)
{
    auto planets = aReg.view<Components::Atmosphere>();
    std::vector<entt::entity> planetEnts;
    std::vector<entt::entity> colonizedPlanetEnts;
    planetEnts.reserve(planets.size());
    colonizedPlanetEnts.reserve(planets.size());
    for (auto& ent : planets)
        planetEnts.push_back(ent);

    std::uniform_int_distribution<size_t> systemDist(0, planetEnts.size() - 1);
    std::uniform_int_distribution<uint8_t> sizeDist(1, 8);
    std::random_device rDev;

    const auto colonies = int(planets.size() * 0.5f);
    for (int i = 0; i < colonies; ++i)
    {
        auto planet = planetEnts[systemDist(rDev)];
        while (std::find(colonizedPlanetEnts.begin(), colonizedPlanetEnts.end(), planet) != colonizedPlanetEnts.end())
            planet = planetEnts[systemDist(rDev)];

        colonizedPlanetEnts.push_back(planet);

        auto& col = aReg.assign<Components::Colony>(planet, sizeDist(rDev));

        printf("[SSG|D] Added colony of size %d on planet %d\n", col.Size, int(aReg.entity(planet)));
    }
}
