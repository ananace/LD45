#include "BaseSystem.hpp"

BaseSystem::BaseSystem()
    : m_application{}
    , m_registry{}
    , m_dispatcher{}
{
}

BaseSystem::~BaseSystem()
{
}

void BaseSystem::init(Application* aApplication, entt::registry& aRegistry, entt::dispatcher& aDispatcher)
{
    m_application = aApplication;
    m_registry = &aRegistry;
    m_dispatcher = &aDispatcher;
}

