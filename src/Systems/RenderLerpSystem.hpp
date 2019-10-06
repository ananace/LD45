#pragma once

#include "../BaseSystem.hpp"

namespace Systems
{

class RenderLerpSystem : public BaseSystem
{
public:
    RenderLerpSystem();
    ~RenderLerpSystem();

    virtual void update(const float aAlpha) override;

private:
    void onInit() override;
};

}
