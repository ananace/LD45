#pragma once

#include "../BaseSystem.hpp"

namespace Systems
{

class RenderSystem : public BaseSystem
{
public:
    RenderSystem();
    ~RenderSystem();

    virtual void update(const float aAlpha) override;

private:
    void onInit() override;
};

}
