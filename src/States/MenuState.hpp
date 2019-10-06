#pragma once

#include "../BaseState.hpp"
#include "../EntityManager.hpp"

#include "../Events/UIButtonClicked.hpp"

namespace Components { struct UIButton; struct UIComponent; }

namespace States
{

class MenuState : public BaseState
{
public:
    MenuState(bool aInGame = false);
    ~MenuState();

    virtual void init() override;
    virtual void handleEvent(const sf::Event& aEvent) override;
    virtual void update(const float aDt) override;
    virtual void fixedUpdate(const float aDt) override;
    virtual void render(const float aAlpha) override;

private:
    void onButtonPress(const Events::UIButtonClicked& aEvent);
    std::tuple<entt::entity, Components::UIComponent&, Components::UIButton&> addButton(const std::string& aTitle);

    EntityManager m_backgroundManager,
                  m_foregroundManager;
    bool m_inGame;
};

}
