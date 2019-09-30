#include "MenuState.hpp"

#include "../Components/UIButton.hpp"
#include "../Components/Renderables.hpp"

#include "../Events/InputEvent.hpp"

#include "../Systems/PhysicsSystem.hpp"
#include "../Systems/RenderSystem.hpp"
#include "../Systems/UIRenderSystem.hpp"

using States::MenuState;

MenuState::MenuState()
    : BaseState("Menu")
{
}

MenuState::~MenuState()
{
}

void MenuState::init()
{
    m_backgroundManager.init(&getApplication());
    m_foregroundManager.init(&getApplication());

    m_backgroundManager.addSystem(std::make_unique<Systems::PhysicsSystem>());
    m_backgroundManager.addRenderSystem(std::make_unique<Systems::RenderSystem>());

    m_foregroundManager.addRenderSystem(std::make_unique<Systems::RenderSystem>());
    m_foregroundManager.addRenderSystem(std::make_unique<Systems::UIRenderSystem>());

    addButton("Play");
    addButton("Settings").Position.top += 50;
    auto& q = addButton("Quit");
    q.Position.top += 100;
    q.Color = { 128, 64, 64 };
}

void MenuState::handleEvent(const sf::Event& aEvent)
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

void MenuState::update(const float aDt)
{
    m_foregroundManager.onUpdate(aDt);
}

void MenuState::fixedUpdate(const float aDt)
{
    m_backgroundManager.getDispatcher().update();
    m_foregroundManager.getDispatcher().update();

    m_backgroundManager.onUpdate(aDt);
}

void MenuState::render(const float aAlpha)
{
    m_backgroundManager.onRender(aAlpha);
    m_foregroundManager.onRender(aAlpha);
}

Components::UIButton& MenuState::addButton(const std::string& aTitle)
{
    auto& r = m_foregroundManager.getRegistry();

    auto button = r.create();
    auto& uib = r.assign<Components::UIButton>(button, aTitle);
    uib.Color = { 64, 128, 64 };
    uib.Position = { 100, 100, 150, 30 };

    return uib;
}
