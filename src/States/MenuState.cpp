#include "MenuState.hpp"

#include "../Events/InputEvent.hpp"

#include "../Systems/PhysicsSystem.hpp"
#include "../Systems/RenderSystem.hpp"

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
}

void MenuState::handleEvent(const sf::Event& aEvent)
{
    switch (aEvent.type)
    {
    case sf::Event::KeyPressed:
    case sf::Event::KeyReleased:
    case sf::Event::MouseButtonPressed:
    case sf::Event::MouseButtonReleased:
    case sf::Event::MouseMoved:
        m_foregroundManager.getDispatcher().enqueue<Events::InputEvent>({ aEvent });
        break;

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
