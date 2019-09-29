#pragma once

#include <entt/entity/registry.hpp>
#include <entt/signal/dispatcher.hpp>

#include <memory>

class Application;
class BaseSystem;

class EntityManager
{
public:
    EntityManager();
    ~EntityManager();

    void init(Application* aApplication);

    void onUpdate(float aDt);
    void onRender(float aAlpha);

    void addSystem(std::unique_ptr<BaseSystem> aSystem);
    void addRenderSystem(std::unique_ptr<BaseSystem> aSystem);

    inline entt::registry& getRegistry() { return m_registry; }
    inline const entt::registry& getRegistry() const { return m_registry; }
    inline entt::dispatcher& getDispatcher() { return m_dispatcher; }
    inline const entt::dispatcher& getDispatcher() const { return m_dispatcher; }

private:
    Application* m_application;
    entt::registry m_registry;
    entt::dispatcher m_dispatcher;

    std::vector<std::unique_ptr<BaseSystem>> m_systems;
    std::vector<std::unique_ptr<BaseSystem>> m_renderSystems;
};
