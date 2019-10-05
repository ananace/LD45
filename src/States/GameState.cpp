#include "GameState.hpp"
#include "../Application.hpp"

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
    // gameView.zoom(0.25f);

    getApplication().getRenderWindow().setView(gameView);

    m_universeManager.onRender(aAlpha);

    getApplication().getRenderWindow().setView(defView);
    m_foregroundManager.onRender(aAlpha);
}

void GameState::createSystem()
{
    std::uniform_real_distribution<float> randAng(0.f, 6.282f);
    std::random_device rDev;

    auto& r = m_universeManager.getRegistry();

    auto sol = r.create<Components::CelestialBody, Components::StarShape, Components::Renderable, Components::Tags::SystemCore, Components::StarField, Components::Position>();
    auto& celestial = std::get<1>(sol);
    auto& star = std::get<2>(sol);
    auto& lerp = std::get<3>(sol);
    auto& field = std::get<5>(sol);
    auto& position = std::get<6>(sol);

    field.FieldSize = sf::FloatRect{ -2000, -2000, 4000, 4000 };
    star.Size = 100.f;
    lerp.LastPosition = position.Position;
    lerp.Position = position.Position;

    auto mercury = r.create<Components::SatteliteBody, Components::PlanetShape, Components::Renderable, Components::Position>();
    {
    auto& sattelite = std::get<1>(mercury);
    auto& planet = std::get<2>(mercury);

    planet.Size = 5.f;
    planet.Color = sf::Color(128, 128, 128);
    sattelite.Orbiting = std::get<0>(sol);
    sattelite.Distance = 150.f;
    sattelite.Speed = 0.025f;

    sattelite.Angle = randAng(rDev);
    }

    auto venus = r.create<Components::SatteliteBody, Components::PlanetShape, Components::Renderable, Components::Position>();
    {
    auto& sattelite = std::get<1>(venus);
    auto& planet = std::get<2>(venus);

    planet.Size = 8.f;
    planet.Color = sf::Color(158, 158, 128);
    sattelite.Orbiting = std::get<0>(sol);
    sattelite.Distance = 200.f;
    sattelite.Speed = 0.030f;

    sattelite.Angle = randAng(rDev);
    }

    auto earth = r.create<Components::SatteliteBody, Components::PlanetShape, Components::Atmosphere, Components::Renderable, Components::Position>();
    {
    auto& sattelite = std::get<1>(earth);
    auto& planet = std::get<2>(earth);
    auto& atmosphere = std::get<3>(earth);

    atmosphere.InnerSize = 15.f;
    atmosphere.OuterSize = 35.f;
    atmosphere.Color = sf::Color(188, 188, 255);

    planet.Size = 15.f;
    planet.Color = sf::Color(158, 158, 228);
    sattelite.Orbiting = std::get<0>(sol);
    sattelite.Distance = 500.f;
    sattelite.Speed = 0.045f;

    sattelite.Angle = randAng(rDev);
    }

    auto luna = r.create<Components::SatteliteBody, Components::PlanetShape, Components::Renderable, Components::Position>();
    {
    auto& sattelite = std::get<1>(luna);
    auto& planet = std::get<2>(luna);

    planet.Size = 5.f;
    planet.Color = sf::Color(128, 128, 128);
    sattelite.Orbiting = std::get<0>(earth);
    sattelite.Distance = 45.f;
    sattelite.Speed = 0.125f;

    sattelite.Angle = randAng(rDev);
    }

    auto mars = r.create<Components::SatteliteBody, Components::PlanetShape, Components::Renderable, Components::Position>();
    {
    auto& sattelite = std::get<1>(mars);
    auto& planet = std::get<2>(mars);

    planet.Size = 12.f;
    planet.Color = sf::Color(228, 198, 96);
    sattelite.Orbiting = std::get<0>(sol);
    sattelite.Distance = 700.f;
    sattelite.Speed = 0.040f;

    sattelite.Angle = randAng(rDev);
    }

    auto jupiter = r.create<Components::SatteliteBody, Components::PlanetShape, Components::Renderable, Components::Position>();
    {
    auto& sattelite = std::get<1>(jupiter);
    auto& planet = std::get<2>(jupiter);

    planet.Size = 25.f;
    planet.Color = sf::Color(228, 198, 128);
    sattelite.Orbiting = std::get<0>(sol);
    sattelite.Distance = 750.f;
    sattelite.Speed = 0.030f;

    sattelite.Angle = randAng(rDev);
    }

    // auto saturn = r.create<Components::SatteliteBody, Components::PlanetShape>();

    // auto uranus = r.create<Components::SatteliteBody, Components::PlanetShape>();

    // auto neptune = r.create<Components::SatteliteBody, Components::PlanetShape>();

    auto gate1 = r.create<Components::SatteliteBody, Components::GateShape, Components::Renderable, Components::Position>();
    {
    auto& sattelite = std::get<1>(gate1);
    auto& gate = std::get<2>(gate1);
    auto& rend = std::get<3>(gate1);

    sattelite.Orbiting = std::get<0>(sol);
    sattelite.Distance = 1000.f;
    sattelite.Speed = 0.0f;

    sattelite.Angle = 1.5f;
    gate.Angle = sattelite.Angle * (180 / 3.1415f);

    rend.LastPosition = rend.Position = std::get<4>(gate1).Position;
    }
}
