#include "GameState.hpp"
#include "../Application.hpp"

#include "../Components/CelestialBody.hpp"
#include "../Components/SatteliteBody.hpp"
#include "../Components/StarShape.hpp"
#include "../Components/PlanetShape.hpp"

#include "../Systems/CelestialRenderSystem.hpp"
#include "../Systems/OrbitalSystem.hpp"
#include "../Systems/PhysicsSystem.hpp"
#include "../Systems/RenderSystem.hpp"
#include "../Systems/UIRenderSystem.hpp"

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


    auto& r = m_universeManager.getRegistry();

    auto sol = r.create<Components::CelestialBody, Components::StarShape>();
    auto& celestial = std::get<1>(sol);
    auto& star = std::get<2>(sol);

    celestial.Position = getApplication().getRenderWindow().getView().getCenter();

    auto mercury = r.create<Components::SatteliteBody, Components::PlanetShape>();
    auto& sattelite = std::get<1>(mercury);
    auto& planet = std::get<2>(mercury);

    planet.Size = 2.439f;
    planet.Color = sf::Color(128, 128, 128);
    sattelite.Orbiting = std::get<0>(sol);
    sattelite.Distance = 57908;
    sattelite.Speed = 47.3f;

    auto venus = r.create<Components::SatteliteBody, Components::PlanetShape>();
    sattelite = std::get<1>(venus);
    planet = std::get<2>(venus);

    planet.Size = 6.051f;
    planet.Color = sf::Color(158, 158, 128);
    sattelite.Orbiting = std::get<0>(sol);
    sattelite.Distance = 108208;
    sattelite.Speed = 35.02f;

    auto earth = r.create<Components::SatteliteBody, Components::PlanetShape>();
    sattelite = std::get<1>(earth);
    planet = std::get<2>(earth);

    planet.Size = 6.371f;
    planet.Color = sf::Color(158, 158, 228);
    sattelite.Orbiting = std::get<0>(sol);
    sattelite.Distance = 149597;
    sattelite.Speed = 29.78f;

    auto luna = r.create<Components::SatteliteBody, Components::PlanetShape>();
    sattelite = std::get<1>(luna);
    planet = std::get<2>(luna);

    planet.Size = 1.737f;
    planet.Color = sf::Color(128, 128, 128);
    sattelite.Orbiting = std::get<0>(earth);
    sattelite.Distance = 383;
    sattelite.Speed = 1.022f;

    auto mars = r.create<Components::SatteliteBody, Components::PlanetShape>();
    sattelite = std::get<1>(mars);
    planet = std::get<2>(mars);

    planet.Size = 3.389f;
    planet.Color = sf::Color(228, 198, 128);
    sattelite.Orbiting = std::get<0>(sol);
    sattelite.Distance = 227950;
    sattelite.Speed = 24.007f;

    // auto jupiter = r.create<Components::SatteliteBody, Components::PlanetShape>();

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
    zoomedView.zoom(0.5f);
    getApplication().getRenderWindow().setView(zoomedView);

    m_universeManager.onRender(aAlpha);

    getApplication().getRenderWindow().setView(defView);
    m_foregroundManager.onRender(aAlpha);
}
