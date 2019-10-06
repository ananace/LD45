#pragma once

#include "../BaseState.hpp"
#include "../EntityManager.hpp"

#include <SFML/System/Vector2.hpp>
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
    void createSystem(const sf::Vector2f aCenter = {});
    void createJumpGates();
    void createJumpHoles();
    void createPlayer();

    EntityManager m_universeManager,
                  m_foregroundManager;
};

}

