#pragma once

#include <entt/entity/registry.hpp>
#include <entt/signal/dispatcher.hpp>

class Application;

class BaseSystem
{
public:
    BaseSystem();
    virtual ~BaseSystem();

    void init(Application* aApplication, entt::registry& aRegistry, entt::dispatcher& aDispatcher);

    inline virtual void update(const float aDt) {}

protected:
    inline virtual void onInit() {}

    inline Application& getApplication() { return *m_application; }
    inline const Application& getApplication() const { return *m_application; }
    inline entt::registry& getRegistry() { return *m_registry; }
    inline const entt::registry& getRegistry() const { return *m_registry; }
    inline entt::dispatcher& getDispatcher() { return *m_dispatcher; }
    inline const entt::dispatcher& getDispatcher() const { return *m_dispatcher; }

private:
    Application* m_application;
    entt::registry* m_registry;
    entt::dispatcher* m_dispatcher;
};
