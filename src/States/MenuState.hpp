#pragma once

#include "../BaseState.hpp"
#include "../EntityManager.hpp"

namespace States
{

class MenuState : public BaseState
{
public:
    MenuState();
    ~MenuState();

    virtual void init() override;
    virtual void handleEvent(const sf::Event& aEvent) override;
    virtual void update(const float aDt) override;
    virtual void fixedUpdate(const float aDt) override;
    virtual void render(const float aAlpha) override;

private:
    EntityManager m_backgroundManager,
                  m_foregroundManager;
};

}
