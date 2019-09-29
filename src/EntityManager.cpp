#include "EntityManager.hpp"
#include "BaseSystem.hpp"

EntityManager::EntityManager()
{
}

EntityManager::~EntityManager()
{
}

void EntityManager::init(Application* aApplication)
{
    m_application = aApplication;
}

void EntityManager::onUpdate(float aDt)
{
    for (auto& sys : m_systems)
        sys->update(aDt);
}
void EntityManager::onRender(float aAlpha)
{
    for (auto& sys : m_renderSystems)
        sys->update(aAlpha);
}

void EntityManager::addSystem(std::unique_ptr<BaseSystem> aSystem)
{
    aSystem->init(m_application, m_registry, m_dispatcher);
    m_systems.push_back(std::move(aSystem));
}
void EntityManager::addRenderSystem(std::unique_ptr<BaseSystem> aSystem)
{
    aSystem->init(m_application, m_registry, m_dispatcher);
    m_renderSystems.push_back(std::move(aSystem));
}
