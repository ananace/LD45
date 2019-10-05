#include "GameState.hpp"
#include "../Application.hpp"

#include "../Components/Atmosphere.hpp"
#include "../Components/CelestialBody.hpp"
#include "../Components/PlanetShape.hpp"
#include "../Components/Renderables.hpp"
#include "../Components/SatteliteBody.hpp"
#include "../Components/StarShape.hpp"

#include "../Systems/CelestialRenderSystem.hpp"
#include "../Systems/OrbitalSystem.hpp"
#include "../Systems/PhysicsSystem.hpp"
#include "../Systems/RenderSystem.hpp"
#include "../Systems/UIRenderSystem.hpp"

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

    m_universeManager.addSystem(std::make_unique<Systems::OrbitalSystem>());
    m_universeManager.addSystem(std::make_unique<Systems::PhysicsSystem>());
    m_universeManager.addRenderSystem(std::make_unique<Systems::CelestialRenderSystem>());
    m_universeManager.addRenderSystem(std::make_unique<Systems::RenderSystem>());

    m_foregroundManager.addRenderSystem(std::make_unique<Systems::RenderSystem>());
    m_foregroundManager.addRenderSystem(std::make_unique<Systems::UIRenderSystem>());

    std::uniform_real_distribution<float> randAng(0.f, 6.282f);
    std::random_device rDev;

    auto& r = m_universeManager.getRegistry();

    auto sol = r.create<Components::CelestialBody, Components::StarShape, Components::LerpedDirectRenderable>();
    auto& celestial = std::get<1>(sol);
    auto& star = std::get<2>(sol);
    auto& lerp = std::get<3>(sol);

    star.Size = 100.f;
    lerp.LastPosition = celestial.Position;
    lerp.Position = celestial.Position;

    auto mercury = r.create<Components::SatteliteBody, Components::PlanetShape, Components::LerpedDirectRenderable>();
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

    auto venus = r.create<Components::SatteliteBody, Components::PlanetShape, Components::LerpedDirectRenderable>();
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

    auto earth = r.create<Components::SatteliteBody, Components::PlanetShape, Components::Atmosphere, Components::LerpedDirectRenderable>();
    {
    auto& sattelite = std::get<1>(earth);
    auto& planet = std::get<2>(earth);
    auto& atmosphere = std::get<3>(earth);

    atmosphere.InnerSize = 15.f;
    atmosphere.OuterSize = 45.f;
    atmosphere.Color = sf::Color(188, 188, 255);

    planet.Size = 15.f;
    planet.Color = sf::Color(158, 158, 228);
    sattelite.Orbiting = std::get<0>(sol);
    sattelite.Distance = 500.f;
    sattelite.Speed = 0.045f;

    sattelite.Angle = randAng(rDev);
    }

    auto luna = r.create<Components::SatteliteBody, Components::PlanetShape, Components::LerpedDirectRenderable>();
    {
    auto& sattelite = std::get<1>(luna);
    auto& planet = std::get<2>(luna);

    planet.Size = 5.f;
    planet.Color = sf::Color(128, 128, 128);
    sattelite.Orbiting = std::get<0>(earth);
    sattelite.Distance = 45.f;
    sattelite.Speed = 0.085f;

    sattelite.Angle = randAng(rDev);
    }

    auto mars = r.create<Components::SatteliteBody, Components::PlanetShape, Components::LerpedDirectRenderable>();
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

    auto jupiter = r.create<Components::SatteliteBody, Components::PlanetShape, Components::LerpedDirectRenderable>();
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
}

void GameState::handleEvent(const sf::Event& aEvent)
{
    switch (aEvent.type)
    {
    case sf::Event::KeyPressed:
        m_foregroundManager.getDispatcher().enqueue<Events::InputEvent<sf::Event::KeyPressed>>({ aEvent }); break;
    case sf::Event::KeyReleased:
        m_foregroundManager.getDispatcher().enqueue<Events::InputEvent<sf::Event::KeyReleased>>({ aEvent }); break;
    case sf::Event::MouseButtonPressed:
        m_foregroundManager.getDispatcher().enqueue<Events::InputEvent<sf::Event::MouseButtonPressed>>({ aEvent }); break;
    case sf::Event::MouseButtonReleased:
        m_foregroundManager.getDispatcher().enqueue<Events::InputEvent<sf::Event::MouseButtonReleased>>({ aEvent }); break;
    case sf::Event::MouseMoved:
        m_foregroundManager.getDispatcher().enqueue<Events::InputEvent<sf::Event::MouseMoved>>({ aEvent }); break;

    default:
        break;
    }
}

void GameState::update(const float aDt)
{
    m_foregroundManager.onUpdate(aDt);
}

void GameState::fixedUpdate(const float aDt)
{
    m_universeManager.getDispatcher().update();
    m_foregroundManager.getDispatcher().update();

    m_universeManager.onUpdate(aDt);
}

void GameState::render(const float aAlpha)
{
    auto defView = getApplication().getRenderWindow().getView();

    auto zoomedView = defView;
    zoomedView.zoom(2.5f);
    zoomedView.setCenter(0,0);
    getApplication().getRenderWindow().setView(zoomedView);

    m_universeManager.onRender(aAlpha);

    getApplication().getRenderWindow().setView(defView);
    m_foregroundManager.onRender(aAlpha);
}
