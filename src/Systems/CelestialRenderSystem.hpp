#pragma once

#include "../BaseSystem.hpp"

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

    std::shared_ptr<sf::Shader> m_atmosphereShader,
                                m_coronaShader;
};

}
