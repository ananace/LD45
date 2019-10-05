#include "GameState.hpp"
#include "../Application.hpp"
#include "../Math.hpp"

#include "../Components/Tags/CameraTag.hpp"
#include "../Components/Tags/SystemCore.hpp"
#include "../Components/Atmosphere.hpp"
#include "../Components/CelestialBody.hpp"
#include "../Components/Friction.hpp"
#include "../Components/GateShape.hpp"
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

    auto& r = m_universeManager.getRegistry();

    auto player = r.create<Components::PlanetShape, Components::Position, Components::Renderable, Components::Friction, Components::Velocity, Components::PlayerInput, Components::Tags::CameraTag>();
    auto& pl = std::get<1>(player);
    auto& phy = std::get<2>(player);
    auto& rend = std::get<3>(player);
    auto& fric = std::get<4>(player);
    auto& cam = std::get<7>(player);

    cam.Influence = 2;
    fric.Friction = 0.25f;
    pl.Color = sf::Color::White;
    pl.Size = 5.f;
    phy.Position.x = 500;
    phy.Position.y = 500;
    rend.Position = phy.Position;
    rend.LastPosition = phy.Position;
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
    printf("Generating %d planets:\n", planets);

    for (; planets > 0; --planets)
    {
        const auto planet = std::get<0>(r.create<Components::Renderable, Components::Position>());
        const float planetSize = planetSizeDist(rDev);
        printf("- Generating planet of size %.1f\n", planetSize);

        const auto& shape = r.assign<Components::PlanetShape>(planet, planetSize, sf::Color(colorDist(rDev), colorDist(rDev), colorDist(rDev)));

        const bool hasAtmosphere = atmosphereChanceDist(rDev) == 0;
        if (hasAtmosphere)
        {
            printf("  with atmosphere\n");
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
            printf("  with %d moons:\n", moons);
            for (uint8_t i = 0; i < moons; ++i)
            {
                auto moon = std::get<0>(r.create<Components::Renderable, Components::Position>());
                float moonSize = moonSizeDist(rDev);
                printf("  - Generating moon of size %.1f\n", moonSize);

                const bool moonHasMoon = moonChanceDist(rDev) == 0 && moonChanceDist(rDev) == 0;
                float additionalRad = 0.f;
                if (moonHasMoon)
                {
                    printf("    with a moon of its own:\n");
                    auto moonMoon = std::get<0>(r.create<Components::Renderable, Components::Position>());
                    float moonMoonSize = moonSizeDist(rDev);

                    printf("    - Generating moon of size %.1f\n", moonMoonSize);

                    const auto& body = r.assign<Components::SatteliteBody>(moonMoon, moon, moonSize + moonMoonSize * stellarSpaceDist(rDev), ((Math::PI * 2.f) / orbitPeriodDist(rDev)), randRadDist(rDev));
                    r.assign<Components::PlanetShape>(moonMoon, moonMoonSize, sf::Color(colorDist(rDev), colorDist(rDev), colorDist(rDev)));
                    printf("      at an orbit of %.2f\n", body.Distance);
                    additionalRad += body.Distance;
                }

                totalMoonRadius += additionalRad + moonSize * stellarSpaceDist(rDev);

                auto& body = r.assign<Components::SatteliteBody>(moon, planet, totalMoonRadius, ((Math::PI * 2.f) / orbitPeriodDist(rDev)), randRadDist(rDev));
                r.assign<Components::PlanetShape>(moon, moonSize, sf::Color(colorDist(rDev), colorDist(rDev), colorDist(rDev)));
                printf("    with an orbital distance of %.2f:\n", body.Distance);
            }
        }

        auto& body = r.assign<Components::SatteliteBody>(planet, systemCenter, currentDistance + totalMoonRadius * 1.75f, ((Math::PI * 2.f) / orbitPeriodDist(rDev)), randRadDist(rDev));
        printf("  with orbit distance of %.2f\n", body.Distance);
        printf("  and orbit period of %.2fs\n\n", (Math::PI * 2.f) / body.Speed);

        currentDistance += totalMoonRadius * 3.5f + planetSize * stellarSpaceDist(rDev);
    }

    printf("Final system size %f\n", currentDistance);

    auto& starfield = r.assign<Components::StarField>(systemCenter);
    starfield.FieldSize.left = aCenter.x - currentDistance * 2.5f;
    starfield.FieldSize.top = aCenter.y - currentDistance * 2.5f;
    starfield.FieldSize.width = currentDistance * 5.f;
    starfield.FieldSize.height = currentDistance * 5.f;

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
