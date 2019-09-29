#pragma once

#include <string>

namespace sf { class RenderWindow; class Event; }

class Application;
class StateManager;

class BaseState
{
public:
    BaseState(const std::string& aName);
    virtual ~BaseState();

    const std::string& getName() const { return m_name; }

    inline virtual void handleEvent(const sf::Event& aEvent) {}
    inline virtual void update(const float aDt) {}
    inline virtual void fixedUpdate(const float aDt) {}
    inline virtual void render(const float aAlpha) {}

protected:
    inline virtual void init() {}

    inline Application& getApplication() { return *m_application; }
    inline const Application& getApplication() const { return *m_application; }
    inline StateManager& getStateManager() { return *m_stateManager; }
    inline const StateManager& getStateManager() const { return *m_stateManager; }
    inline sf::RenderWindow& getRenderWindow() { return *m_renderWindow; }
    inline const sf::RenderWindow& getRenderWindow() const { return *m_renderWindow; }
private:
    void setApplication(Application* aApp);

    Application* m_application;
    std::string m_name;

    StateManager* m_stateManager;
    sf::RenderWindow* m_renderWindow;

    friend class StateManager;
};
