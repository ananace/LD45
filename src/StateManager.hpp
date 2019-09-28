#pragma once

#include <memory>
#include <deque>

class Application;
class BaseState;

class StateManager
{
public:
    StateManager(Application& aApp);
    ~StateManager();

    bool hasState(const std::string& aName) const;
    bool pushState(std::unique_ptr<BaseState> aState, bool aSwitch = true);
    bool popState(const std::string& aName);
    bool changeState(const std::string& aName);

    inline BaseState* getCurrent() { return m_curState; }
    inline const BaseState* getCurrent() const { return m_curState; }

private:
    Application& m_application;
    std::deque<std::unique_ptr<BaseState>> m_states;
    BaseState* m_curState;
};
