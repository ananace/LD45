#include "GameState.hpp"
#include "../Application.hpp"
#include "../Math.hpp"
#include "../Util.hpp"

#include "../Components/Tags/CameraTag.hpp"
#include "../Components/Tags/InSystem.hpp"
#include "../Components/Tags/JumpCapable.hpp"
#include "../Components/Tags/SystemCore.hpp"
#include "../Components/Tags/TracedOrbit.hpp"
#include "../Components/Atmosphere.hpp"
#include "../Components/CelestialBody.hpp"
#include "../Components/Friction.hpp"
#include "../Components/GateShape.hpp"
#include "../Components/JumpConnection.hpp"
#include "../Components/Position.hpp"
#include "../Components/PlanetShape.hpp"
#include "../Components/PlayerInput.hpp"
#include "../Components/Renderables.hpp"
#include "../Components/SatteliteBody.hpp"
#include "../Components/StarField.hpp"
#include "../Components/StarShape.hpp"
#include "../Components/Velocity.hpp"

#include "../Systems/CelestialRenderSystem.hpp"
#include "../Systems/InputSystem.hpp"
#include "../Systems/LogicSystem.hpp"
#include "../Systems/OrbitalSystem.hpp"
#include "../Systems/PhysicsSystem.hpp"
#include "../Systems/RenderSystem.hpp"
#include "../Systems/UIRenderSystem.hpp"

#include <SFML/Graphics/Color.hpp>
#include <algorithm>
#include <random>

using States::GameState;

float zoom = 1.f;

GameState::GameState()
    : BaseState("Game")
{
}

GameState::~GameState()
{
}

void GameState::init()
{
    m_universeManager.init(&getApplication());
    m_foregroundManager.init(&getApplication());

    m_universeManager.addSystem(std::make_unique<Systems::InputSystem>());
    m_universeManager.addSystem(std::make_unique<Systems::LogicSystem>());
    m_universeManager.addSystem(std::make_unique<Systems::OrbitalSystem>());
    m_universeManager.addSystem(std::make_unique<Systems::PhysicsSystem>());
    m_universeManager.addRenderSystem(std::make_unique<Systems::CelestialRenderSystem>());
    m_universeManager.addRenderSystem(std::make_unique<Systems::RenderSystem>());

    m_foregroundManager.addRenderSystem(std::make_unique<Systems::RenderSystem>());
    m_foregroundManager.addRenderSystem(std::make_unique<Systems::UIRenderSystem>());

    createSystem();
    for (int i = 0; i < 8; ++i)
    {
        const float total = (Math::PI*2) / 8;
        float dir = total * i;
        createSystem(sf::Vector2f(std::cos(dir), std::sin(dir)) * 50000.f);
    }
    for (int i = 0; i < 16; ++i)
    {
        const float total = (Math::PI*2) / 16;
        float dir = total * i;
        createSystem(sf::Vector2f(std::cos(dir), std::sin(dir)) * 100000.f);
    }
    for (int i = 0; i < 32; ++i)
    {
        const float total = (Math::PI*2) / 32;
        float dir = total * i;
        createSystem(sf::Vector2f(std::cos(dir), std::sin(dir)) * 150000.f);
    }

    createJumpGates();
    createJumpHoles();
    createPlayer();
}

void GameState::handleEvent(const sf::Event& aEvent)
{
    switch (aEvent.type)
    {
    case sf::Event::KeyPressed:
        m_universeManager.getDispatcher().enqueue<Events::InputEvent<sf::Event::KeyPressed>>({ aEvent });
        m_foregroundManager.getDispatcher().enqueue<Events::InputEvent<sf::Event::KeyPressed>>({ aEvent });
        break;
    case sf::Event::KeyReleased:
        m_universeManager.getDispatcher().enqueue<Events::InputEvent<sf::Event::KeyReleased>>({ aEvent });
        m_foregroundManager.getDispatcher().enqueue<Events::InputEvent<sf::Event::KeyReleased>>({ aEvent });
        break;
    case sf::Event::MouseButtonPressed:
        m_universeManager.getDispatcher().enqueue<Events::InputEvent<sf::Event::MouseButtonPressed>>({ aEvent });
        m_foregroundManager.getDispatcher().enqueue<Events::InputEvent<sf::Event::MouseButtonPressed>>({ aEvent });
        break;
    case sf::Event::MouseButtonReleased:
        m_universeManager.getDispatcher().enqueue<Events::InputEvent<sf::Event::MouseButtonReleased>>({ aEvent });
        m_foregroundManager.getDispatcher().enqueue<Events::InputEvent<sf::Event::MouseButtonReleased>>({ aEvent });
        break;
    case sf::Event::MouseWheelScrolled:
        if (aEvent.mouseWheelScroll.delta < 0)
            zoom *= 1.15f;
        else
            zoom *= 0.95f;

        m_universeManager.getDispatcher().enqueue<Events::InputEvent<sf::Event::MouseWheelScrolled>>({ aEvent });
        m_foregroundManager.getDispatcher().enqueue<Events::InputEvent<sf::Event::MouseWheelScrolled>>({ aEvent });
        break;
    case sf::Event::MouseMoved:
        m_universeManager.getDispatcher().enqueue<Events::InputEvent<sf::Event::MouseMoved>>({ aEvent });
        m_foregroundManager.getDispatcher().enqueue<Events::InputEvent<sf::Event::MouseMoved>>({ aEvent });
        break;

    default:
        break;
    }
}

void GameState::update(const float aDt)
{
    m_foregroundManager.getDispatcher().update();
    m_foregroundManager.onUpdate(aDt);
}

void GameState::fixedUpdate(const float aDt)
{
    m_universeManager.getDispatcher().update();
    m_universeManager.onUpdate(aDt);
}

void GameState::render(const float aAlpha)
{
    auto defView = getApplication().getRenderWindow().getView();
    auto gameView = defView;

    sf::Vector2f cameraPosition;
    size_t cameraSources{};

    auto& r = m_universeManager.getRegistry();
    auto v = r.group<const Components::Tags::CameraTag>(entt::get<const Components::Renderable>);
    v.each([&cameraSources, &cameraPosition](auto ent, const auto& cam, const auto& renderable) {
        cameraPosition += renderable.CurrentPosition * float(cam.Influence);
        cameraSources += cam.Influence;
    });
    const auto vExtraEnd = v.raw<const Components::Tags::CameraTag>() + v.size<const Components::Tags::CameraTag>();
    const auto vExtraBegin = v.raw<const Components::Tags::CameraTag>() + v.size();

    std::for_each(vExtraBegin, vExtraEnd, [&r, &cameraSources, cameraPosition](auto& cam) {
        // auto ent = r.entity(cam);
    });

    cameraPosition /= float(cameraSources);

    gameView.setCenter(cameraPosition);
    gameView.zoom(zoom);

    getApplication().getRenderWindow().setView(gameView);

    m_universeManager.onRender(aAlpha);

    getApplication().getRenderWindow().setView(defView);
    m_foregroundManager.onRender(aAlpha);
}

void GameState::createSystem(const sf::Vector2f aCenter)
{
    auto& r = m_universeManager.getRegistry();

    std::uniform_real_distribution<float> randRadDist(0.f, Math::PI * 2.f);
    std::uniform_real_distribution<float> starSizeDist(90.f, 200.f);
    std::uniform_real_distribution<float> planetSizeDist(4.f, 50.f);
    std::uniform_real_distribution<float> moonSizeDist(3.f, 8.f);
    std::uniform_real_distribution<float> stellarSpaceDist(1.75f, 8.f);
    std::uniform_real_distribution<float> atmosphereThicknessDist(1.0f, 2.5f);
    std::uniform_int_distribution<uint8_t> starTypeDist(Components::StarShape::O, Components::StarShape::M);
    std::uniform_int_distribution<int> binarySystemChanceDist(0, 2);
    std::uniform_int_distribution<int> moonChanceDist(0, 3);
    std::uniform_int_distribution<int> atmosphereChanceDist(0, 5);
    std::uniform_int_distribution<uint8_t> planetCountDist(0, 10);
    std::uniform_int_distribution<uint8_t> moonCountDist(1, 6);
    std::uniform_int_distribution<uint8_t> colorDist(0, 255);

    // 2.5m - 15m for a full orbit
    std::uniform_real_distribution<float> orbitPeriodDist(2.5f * 60.f, 15.f * 60.f);

    std::random_device rDev;

    bool binarySystem = binarySystemChanceDist(rDev) == 0;

    auto systemCenter = r.create();
    r.assign<Components::Position>(systemCenter, aCenter);
    r.assign<Components::Tags::SystemCore>(systemCenter);

    float currentDistance = 0;

    if (binarySystem)
    {
        printf("Generating Binary system\n");
        auto starType1 = Components::StarShape::StarType(starTypeDist(rDev));
        auto starSize1 = starSizeDist(rDev);
        auto starType2 = Components::StarShape::StarType(starTypeDist(rDev));
        auto starSize2 = starSizeDist(rDev);

        float orbitTime = ((Math::PI * 2.f) / orbitPeriodDist(rDev));

        float ang = randRadDist(rDev);

        // Orbit points, in the centerpoint between system center and the star
        auto star1Orbit = std::get<0>(r.create<Components::Position>());
        r.assign<Components::SatteliteBody>(star1Orbit, systemCenter, ((starSize1 + starSize2) / 2) * 1.5f, orbitTime, ang);

        auto star2Orbit = std::get<0>(r.create<Components::Position>());
        r.assign<Components::SatteliteBody>(star2Orbit, systemCenter, ((starSize1 + starSize2) / 2) * 1.5f, orbitTime, ang + Math::PI);

        auto star1 = std::get<0>(r.create<Components::CelestialBody, Components::Position, Components::Renderable>());
        r.assign<Components::SatteliteBody>(star1, star1Orbit, ((starSize1 + starSize2) / 2), orbitTime, ang + Math::PI * 1.5f);
        r.assign<Components::StarShape>(star1, starSize1, starType1);

        auto star2 = std::get<0>(r.create<Components::CelestialBody, Components::Position, Components::Renderable>());
        r.assign<Components::SatteliteBody>(star2, star2Orbit, ((starSize1 + starSize2) / 2), -orbitTime, ang - Math::PI * 1.5f);
        r.assign<Components::StarShape>(star2, starSize2, starType2);

        currentDistance += ((starSize1 + starSize2) / 2) * stellarSpaceDist(rDev);
    }
    else
    {
        printf("Generating system\n");
        auto starType = Components::StarShape::StarType(starTypeDist(rDev));

        r.assign<Components::CelestialBody>(systemCenter);
        auto& shape = r.assign<Components::StarShape>(systemCenter, starSizeDist(rDev), starType);
        auto& rend = r.assign<Components::Renderable>(systemCenter);
        rend.Position = rend.LastPosition = aCenter;

        currentDistance += shape.Size * stellarSpaceDist(rDev);
    }

    auto planets = planetCountDist(rDev);
    printf("[GameState|D] Generating %d planets:\n", planets);

    for (; planets > 0; --planets)
    {
        const auto planet = std::get<0>(r.create<Components::Position, Components::Renderable, Components::Tags::TracedOrbit>());
        const float planetSize = planetSizeDist(rDev);
        printf("[GameState|D] - Generating planet of size %.1f\n", planetSize);

        const auto& shape = r.assign<Components::PlanetShape>(planet, planetSize, sf::Color(colorDist(rDev), colorDist(rDev), colorDist(rDev)));

        const bool hasAtmosphere = atmosphereChanceDist(rDev) == 0;
        if (hasAtmosphere)
        {
            printf("[GameState|D]   with atmosphere\n");
            const auto& atmos = r.assign<Components::Atmosphere>(planet, planetSize, planetSize * atmosphereThicknessDist(rDev), shape.Color);

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
            printf("[GameState|D]   with %d moons:\n", moons);
            for (uint8_t i = 0; i < moons; ++i)
            {
                auto moon = std::get<0>(r.create<Components::Position, Components::Renderable, Components::Tags::TracedOrbit>());
                float moonSize = moonSizeDist(rDev);
                printf("[GameState|D]   - Generating moon of size %.1f\n", moonSize);

                const bool moonHasMoon = moonChanceDist(rDev) == 0 && moonChanceDist(rDev) == 0;
                float additionalRad = 0.f;
                if (moonHasMoon)
                {
                    printf("[GameState|D]     with a moon of its own:\n");
                    auto moonMoon = std::get<0>(r.create<Components::Position, Components::Renderable, Components::Tags::TracedOrbit>());
                    float moonMoonSize = moonSizeDist(rDev);

                    printf("[GameState|D]     - Generating moon of size %.1f\n", moonMoonSize);

                    const auto& body = r.assign<Components::SatteliteBody>(moonMoon, moon, moonSize + moonMoonSize * stellarSpaceDist(rDev), ((Math::PI * 2.f) / orbitPeriodDist(rDev)), randRadDist(rDev));
                    r.assign<Components::PlanetShape>(moonMoon, moonMoonSize, sf::Color(colorDist(rDev), colorDist(rDev), colorDist(rDev)));
                    printf("[GameState|D]       at an orbit of %.2f\n", body.Distance);
                    additionalRad += body.Distance;
                }

                totalMoonRadius += additionalRad + moonSize * stellarSpaceDist(rDev);

                auto& body = r.assign<Components::SatteliteBody>(moon, planet, totalMoonRadius, ((Math::PI * 2.f) / orbitPeriodDist(rDev)), randRadDist(rDev));
                r.assign<Components::PlanetShape>(moon, moonSize, sf::Color(colorDist(rDev), colorDist(rDev), colorDist(rDev)));
                printf("[GameState|D]     with an orbital distance of %.2f:\n", body.Distance);
            }
        }

        auto& body = r.assign<Components::SatteliteBody>(planet, systemCenter, currentDistance + totalMoonRadius * 1.75f, ((Math::PI * 2.f) / orbitPeriodDist(rDev)), randRadDist(rDev));
        printf("[GameState|D]   with orbit distance of %.2f\n", body.Distance);
        printf("[GameState|D]   and orbit period of %.2fs\n\n", (Math::PI * 2.f) / body.Speed);

        currentDistance += totalMoonRadius * 3.5f + planetSize * stellarSpaceDist(rDev);
    }

    printf("[GameState|D] Final system size %f\n", currentDistance);

    currentDistance = std::max(currentDistance, 5000.f);

    auto& starfield = r.assign<Components::StarField>(systemCenter);
    starfield.Count = 2048;
    starfield.FieldSize.left = aCenter.x - currentDistance * 2.5f;
    starfield.FieldSize.top = aCenter.y - currentDistance * 2.5f;
    starfield.FieldSize.width = currentDistance * 5.f;
    starfield.FieldSize.height = currentDistance * 5.f;
}

void GameState::createJumpGates()
{
    auto& r = m_universeManager.getRegistry();

    auto systems = r.view<Components::Tags::SystemCore, Components::Position>();
    std::vector<entt::entity> systemEnts;
    systemEnts.reserve(systems.size());
    for (auto& ent : systems)
        systemEnts.push_back(ent);

    std::uniform_int_distribution<size_t> systemDist(0, systemEnts.size());
    std::uniform_real_distribution<float> satteliteOffsetDist(-(Math::PI / 8.f), +(Math::PI / 8.f));
    std::random_device rDev;

    const auto gates = int(systems.size() * 0.75f);
    for (int i = 0; i < gates; ++i)
    {
        auto sysEnt1 = systemEnts[systemDist(rDev)];
        auto sysEnt2 = sysEnt1;
        while (sysEnt1 == sysEnt2)
            sysEnt2 = systemEnts[systemDist(rDev)];

        auto& sysPos1 = systems.get<Components::Position>(sysEnt1);
        auto& sysPos2 = systems.get<Components::Position>(sysEnt2);

        auto gate1 = std::get<0>(r.create<Components::Renderable, Components::Position>());
        auto gate2 = std::get<0>(r.create<Components::Renderable, Components::Position>());

        auto dir1to2 = Util::GetAngle(Util::GetNormalized(sysPos2.Position - sysPos1.Position));
        auto dir2to1 = Util::GetAngle(Util::GetNormalized(sysPos1.Position - sysPos2.Position));

        r.assign<Components::SatteliteBody>(gate1, sysEnt1, 2500.f, 0.f, dir1to2 + satteliteOffsetDist(rDev));
        r.assign<Components::GateShape>(gate1, dir1to2);
        r.assign<Components::JumpConnection>(gate1, gate2, 10.f);

        r.assign<Components::SatteliteBody>(gate2, sysEnt2, 2500.f, 0.f, dir2to1 + satteliteOffsetDist(rDev));
        r.assign<Components::GateShape>(gate2, dir2to1);
        r.assign<Components::JumpConnection>(gate2, gate1, 10.f);

        printf("[GameState|D] Adding jumpgate between systems %d and %d\n", int(r.entity(sysEnt1)), int(r.entity(sysEnt2)));
    }
/*
    auto gate1 = r.create<Components::SatteliteBody, Components::GateShape, Components::Renderable, Components::Position>();
    {
    auto& sattelite = std::get<1>(gate1);
    auto& gate = std::get<2>(gate1);
    auto& rend = std::get<3>(gate1);

    sattelite.Orbiting = std::get<0>(sol);
    sattelite.Distance = 1000.f;
    sattelite.Speed = 0.0f;

    sattelite.Angle = 1.5f;
    gate.Angle = sattelite.Angle * Math::RAD2DEG;

    rend.LastPosition = rend.Position = std::get<4>(gate1).Position;
    }
*/
}

void GameState::createJumpHoles()
{
}

void GameState::createPlayer()
{
    auto& r = m_universeManager.getRegistry();

    auto player = std::get<0>(r.create<Components::Renderable, Components::PlayerInput, Components::Tags::CameraTag, Components::Tags::JumpCapable, Components::Velocity>());
    // r.assign<Components::Tags::InSystem>(player, entt::entity(0));

    auto& pl = r.assign<Components::PlanetShape>(player);
    auto& phy = r.assign<Components::Position>(player);
    auto& fric = r.assign<Components::Friction>(player);

    fric.Friction = 0.25f;
    pl.Color = sf::Color::White;
    pl.Size = 5.f;
    phy.Position.x = 500;
    phy.Position.y = 500;
}
