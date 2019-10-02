#include "BaseState.hpp"
#include "StateManager.hpp"
#include "Util.hpp"

StateManager::StateManager()
    : m_application{}
    , m_curState{}
{

}

StateManager::~StateManager()
{
}

void StateManager::init(Application* aApplication)
{
    m_application = aApplication;
}

bool StateManager::hasState(const std::string& aName) const
{
    for (auto& state : m_states)
        if (state->getName() == aName)
            return true;
    return false;
}

bool StateManager::pushState(std::unique_ptr<BaseState> aState, uint8_t aFlags)
{
    auto name = aState->getName();
    aState->setApplication(m_application);
    aState->init();

    m_states.push_front(std::move(aState));

    if ((aFlags & State_SwitchTo) == State_SwitchTo)
        return changeState(name, aFlags);
    else
        return true;
}

bool StateManager::popState(const std::string& aName)
{
    for (auto it = m_states.begin(); it != m_states.end(); ++it)
    {
        auto& curState = *it;
        if (curState->getName() == aName)
        {
            m_curState = nullptr;
            it = m_states.erase(it);

            if (it != m_states.end())
                m_curState = it->get();

            return true;
        }
    }

    return false;
}

bool StateManager::changeState(const std::string& aName, uint8_t aFlags)
{
    for (auto it = m_states.begin(); it != m_states.end(); ++it)
    {
        auto& state = *it;
        if (state->getName() == aName)
        {
            if ((aFlags & State_PopSelf) == State_PopSelf)
            {
                auto curIt = std::find_if(m_states.begin(), m_states.end(), [&](auto& test) { return test.get() == m_curState; });
                m_states.erase(curIt);
            }

            m_curState = state.get();

            if ((aFlags & State_BringToFront) == State_BringToFront)
            {
                m_states.erase(it);
                m_states.push_front(std::move(state));
            }

            return true;
        }
    }

    return false;
}
