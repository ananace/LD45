#pragma once

#include "../BaseSystem.hpp"
#include <entt/resource/handle.hpp>

namespace sf { class Shader; }

namespace Systems
{

class CelestialRenderSystem : public BaseSystem
{
public:
    CelestialRenderSystem();
    ~CelestialRenderSystem();

    virtual void update(const float aAlpha) override;

private:
    void onInit() override;

    entt::resource_handle<sf::Shader> m_atmosphereShader,
                                      m_coronaShader,
                                      m_orbitShader;
};

}
