#include "MenuState.hpp"
#include "../Application.hpp"

#include "../Components/UIButton.hpp"
#include "../Components/UIComponent.hpp"
#include "../Components/UIDialog.hpp"
#include "../Components/Renderables.hpp"

#include "../Events/InputEvent.hpp"

#include "../Systems/PhysicsSystem.hpp"
#include "../Systems/RenderSystem.hpp"
#include "../Systems/UIRenderSystem.hpp"

#include "GameState.hpp"
#include <SFML/Window/Keyboard.hpp>
using States::MenuState;

MenuState::MenuState(bool aInGame)
    : BaseState("Menu")
    , m_inGame(aInGame)
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

    auto& d = m_foregroundManager.getDispatcher();
    d.sink<Events::UIButtonClicked>().connect<&MenuState::onButtonPress>(*this);

    auto& r = m_foregroundManager.getRegistry();

    auto menuDial = r.create<Components::UIComponent, Components::UIDialog>();
    {
        auto& uid = std::get<2>(menuDial);
        auto& uic = std::get<1>(menuDial);

        uid.Color = sf::Color(0, 128, 128, 196);
        uic.Position = { -300, -270, 170, 100 };

        if (m_inGame)
            uic.Position.height += 50;
    }

    auto p = addButton("Start New");
    std::get<1>(p).Parent = std::get<0>(menuDial);

    if (m_inGame)
    {
        auto r = addButton("Resume");
        std::get<1>(r).Parent = std::get<0>(menuDial);
        std::get<1>(r).Position.top += 50;
        std::get<2>(r).Color = { 128, 128, 64 };
    }

    auto q = addButton("Quit");
    auto& uib = std::get<2>(q);
    auto& uic = std::get<1>(q);
    uic.Parent = std::get<0>(menuDial);
    uic.Position.top += 50;
    if (m_inGame)
        uic.Position.top += 50;
    uib.Color = { 128, 64, 64 };
}

void MenuState::handleEvent(const sf::Event& aEvent)
{
    switch (aEvent.type)
    {
    case sf::Event::KeyPressed:
        m_foregroundManager.getDispatcher().enqueue<Events::InputEvent<sf::Event::KeyPressed>>({ aEvent }); break;
    case sf::Event::KeyReleased:
        if (aEvent.key.code == sf::Keyboard::Escape)
        {
            if (m_inGame)
                getApplication().getStateManager().changeState("Game", StateManager::State_PopSelf);
            else
                getApplication().getRenderWindow().close();
            return;
        }
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

void MenuState::onButtonPress(const Events::UIButtonClicked& aEvent)
{
    if (aEvent.Button == "Quit")
        getApplication().getRenderWindow().close();
    else if (aEvent.Button == "Start New")
        getApplication().getStateManager().pushState(std::make_unique<States::GameState>(), StateManager::State_SwitchTo | StateManager::State_PopSelf | StateManager::State_PopOther);
    else if (aEvent.Button == "Resume")
        getApplication().getStateManager().changeState("Game", StateManager::State_PopSelf);
}

std::tuple<entt::entity, Components::UIComponent&, Components::UIButton&> MenuState::addButton(const std::string& aTitle)
{
    auto& r = m_foregroundManager.getRegistry();

    auto button = r.create<Components::UIComponent, Components::UIButton>();
    auto& uib = std::get<2>(button);
    auto& uic = std::get<1>(button);

    uib.Text = aTitle;
    uib.Color = { 64, 128, 64 };
    uic.Position = { 10, 10, 150, 30 };

    return button;
}
