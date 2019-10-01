#pragma once

#include <memory>
#include <deque>

class Application;
class BaseState;

class StateManager
{
public:
    enum ChangeFlags : uint8_t
    {
        State_None = 0,
        State_SwitchTo     = 1 << 0,
        State_BringToFront = 1 << 1,
        State_PopSelf      = 1 << 2,
    };

    StateManager();
    ~StateManager();

    void init(Application* aApplication);

    bool hasState(const std::string& aName) const;
    bool pushState(std::unique_ptr<BaseState> aState, uint8_t aFlags = State_SwitchTo);
    bool popState(const std::string& aName);
    bool changeState(const std::string& aName, uint8_t aFlags = State_SwitchTo);

    inline BaseState* getCurrent() { return m_curState; }
    inline const BaseState* getCurrent() const { return m_curState; }

private:
    Application* m_application;
    std::deque<std::unique_ptr<BaseState>> m_states;
    BaseState* m_curState;
};
