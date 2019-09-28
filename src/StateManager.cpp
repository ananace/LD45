#include "BaseState.hpp"
#include "StateManager.hpp"
#include "Util.hpp"

StateManager::StateManager(Application& aApp)
    : m_application(aApp)
    , m_curState{}
{

}

StateManager::~StateManager()
{
}

bool StateManager::hasState(const std::string& aName) const
{
    for (auto& state : m_states)
        if (state->getName() == aName)
            return true;
    return false;
}

bool StateManager::pushState(std::unique_ptr<BaseState> aState, bool aSwitch)
{
    auto name = aState->getName();
    aState->setApplication(m_application);
    aState->init();

    m_states.push_back(std::move(aState));

    if (aSwitch)
        return changeState(name);
    else
        return true;
}

bool StateManager::popState(const std::string& aName)
{
    for (size_t it = m_states.size() - 1; it >= 0; --it)
    {
        auto& curState = m_states[it];
        if (curState->getName() == aName)
        {
            m_states.erase(m_states.begin() + it);
            return true;
        }
    }

    return false;
}

bool StateManager::changeState(const std::string& aName)
{
    for (auto& state : Util::reverse(m_states))
    {
        if (state->getName() == aName)
        {
            m_curState = state.get();
            return true;
        }
    }

    return false;
}
