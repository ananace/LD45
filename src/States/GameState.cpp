#include "GameState.hpp"
#include "../Application.hpp"
#include "../Math.hpp"
#include "../StarSystemGenerator.hpp"
#include "../Util.hpp"

#include "../Components/Tags/CameraTag.hpp"
#include "../Components/Tags/JumpCapable.hpp"
#include "../Components/Friction.hpp"
#include "../Components/PlayerInput.hpp"
#include "../Components/Position.hpp"
#include "../Components/Renderables.hpp"
#include "../Components/Rotation.hpp"
#include "../Components/Velocity.hpp"

#include "../Ships/BasicShip.hpp"

#include "../Systems/CelestialRenderSystem.hpp"
#include "../Systems/InputSystem.hpp"
#include "../Systems/LogicSystem.hpp"
#include "../Systems/OrbitalSystem.hpp"
#include "../Systems/PhysicsSystem.hpp"
#include "../Systems/RenderLerpSystem.hpp"
#include "../Systems/RenderSystem.hpp"
#include "../Systems/UIRenderSystem.hpp"

#include "MenuState.hpp"
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <algorithm>
#include <memory>
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
    m_universeManager.addRenderSystem(std::make_unique<Systems::RenderLerpSystem>());

    m_foregroundManager.addRenderSystem(std::make_unique<Systems::RenderSystem>());
    m_foregroundManager.addRenderSystem(std::make_unique<Systems::UIRenderSystem>());

    StarSystemGenerator ssg;

    ssg.createSystem(m_universeManager.getRegistry());
    for (int i = 0; i < kSystemRings; ++i)
    {
        const float distance = (i + 1) * 50000.f;
        const int systems = std::pow(2, i + 3);

        for (int j = 0; j < systems; ++j)
        {
            const float total = (Math::PI*2) / systems;
            float dir = total * j;
            ssg.createSystem(m_universeManager.getRegistry(), sf::Vector2f(std::cos(dir), std::sin(dir)) * distance);
        }
    }

    ssg.createJumpGates(m_universeManager.getRegistry());
    ssg.createJumpHoles(m_universeManager.getRegistry());
    ssg.createColonies(m_universeManager.getRegistry());
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
        if (aEvent.key.code == sf::Keyboard::Escape)
        {
            getApplication().getStateManager().pushState(std::make_unique<MenuState>(true), StateManager::State_SwitchTo);
            return;
        }

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
        zoom = std::clamp(zoom, 0.25f, 2.5f);

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

Ships::BasicShip playerShip;
void GameState::createPlayer()
{
    auto& r = m_universeManager.getRegistry();

    auto player = std::get<0>(r.create<Components::Renderable, Components::PlayerInput, Components::Tags::CameraTag, Components::Tags::JumpCapable, Components::Velocity, Components::Rotation>());

    r.assign<Components::DrawableRenderable>(player, &playerShip);

    // auto& pl = r.assign<Components::PlanetShape>(player);
    auto& phy = r.assign<Components::Position>(player);
    auto& fric = r.assign<Components::Friction>(player);

    // pl.Color = sf::Color::White;
    // pl.Size = 5.f;

    fric.Friction = 0.25f;
    phy.Position.x = 500;
    phy.Position.y = 500;
}
