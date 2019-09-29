#pragma once

#include "../BaseSystem.hpp"

namespace Systems
{

class UIRenderSystem : public BaseSystem
{
public:
    UIRenderSystem();
    ~UIRenderSystem();

    virtual void update(const float aAlpha);
};

}
