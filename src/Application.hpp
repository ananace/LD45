#pragma once

#include "ResourceManager.hpp"
#include "StateManager.hpp"

#include <SFML/Graphics/RenderWindow.hpp>

#include <chrono>

class Application
{
public:
    enum
    {
        kTickRate = 60,
        kTargetDt = 6,
    };

    static constexpr float kTickLength = 1.f / kTickRate;

    Application();
    Application(const Application&) = delete;
    Application(Application&&) = delete;
    ~Application();

    Application& operator=(const Application&) = delete;
    Application& operator=(Application&&) = delete;

    inline sf::RenderWindow& getRenderWindow() { return m_window; }
    inline ResourceManager& getResourceManager() { return m_resourceManager; }
    inline StateManager& getStateManager() { return m_stateManager; }
    inline std::chrono::high_resolution_clock::duration getTotalTime() const { return m_total; }

    void run();
    void stop();

private:
    std::chrono::high_resolution_clock::duration m_total;
    sf::RenderWindow m_window;
    sf::View m_defaultView;
    ResourceManager m_resourceManager;
    StateManager m_stateManager;
};
