#include "Application.hpp"
#include "BaseState.hpp"
#include "StateManager.hpp"

BaseState::BaseState(const std::string& aName)
    : m_application{}
    , m_name(aName)
    , m_stateManager{}
    , m_renderWindow{}
{
}

BaseState::~BaseState()
{
}

void BaseState::setApplication(Application& aApp)
{
    m_application = &aApp;

    m_stateManager = &aApp.getStateManager();
    m_renderWindow = &aApp.getRenderWindow();
}
