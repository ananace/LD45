#pragma once

#include "../BaseState.hpp"
#include "../EntityManager.hpp"

namespace States
{

class GameState : public BaseState
{
public:
    GameState();
    ~GameState();

    virtual void init() override;
    virtual void handleEvent(const sf::Event& aEvent) override;
    virtual void update(const float aDt) override;
    virtual void fixedUpdate(const float aDt) override;
    virtual void render(const float aAlpha) override;

private:
    EntityManager m_universeManager,
                  m_foregroundManager;
};

}

