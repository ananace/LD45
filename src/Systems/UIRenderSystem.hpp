#pragma once

#include "../BaseSystem.hpp"

#include "../Events/InputEvent.hpp"

namespace Systems
{

class UIRenderSystem : public BaseSystem
{
public:
    UIRenderSystem();
    ~UIRenderSystem();

    virtual void update(const float aAlpha);
    virtual void onInit();

private:
    void onMouseMove(const Events::InputEvent<sf::Event::MouseMoved>& ev);
    void onMousePress(const Events::InputEvent<sf::Event::MouseButtonPressed>& ev);
    void onMouseRelease(const Events::InputEvent<sf::Event::MouseButtonReleased>& ev);
};

}
