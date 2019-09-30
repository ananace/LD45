#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include "ResourceManager.hpp"
#include "StateManager.hpp"

class Application
{
public:
    enum
    {
        kTickRate = 60
    };

    Application();
    Application(const Application&) = delete;
    Application(Application&&) = delete;
    ~Application();

    Application& operator=(const Application&) = delete;
    Application& operator=(Application&&) = delete;

    inline sf::RenderWindow& getRenderWindow() { return m_window; }
    inline ResourceManager& getResourceManager() { return m_resourceManager; }
    inline StateManager& getStateManager() { return m_stateManager; }

    void run();
    void stop();

private:
    sf::RenderWindow m_window;
    sf::View m_defaultView;
    ResourceManager m_resourceManager;
    StateManager m_stateManager;
};