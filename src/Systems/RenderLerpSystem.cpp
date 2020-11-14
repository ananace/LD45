#include "RenderLerpSystem.hpp"
#include "../Application.hpp"
#include "../Math.hpp"
#include "../Util.hpp"

#include "../Components/Position.hpp"
#include "../Components/Renderables.hpp"
#include "../Components/Rotation.hpp"

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>

#include <entt/entity/fwd.hpp>
#include <gsl/gsl_assert>

#include <chrono>
#include <gsl/gsl_util>

using Systems::RenderLerpSystem;
using namespace Components;

void onRenderableConstruct(entt::registry& aRegistry, entt::entity aEnt)
{
    auto& aRenderable = aRegistry.get<Renderable>(aEnt);
    if (aRegistry.has<Position>(aEnt))
    {
        auto& pos = aRegistry.get<Position>(aEnt);
        aRenderable.Position = aRenderable.LastPosition = pos.Position;
    }
    if (aRegistry.has<Rotation>(aEnt))
    {
        auto& rot = aRegistry.get<Rotation>(aEnt);
        aRenderable.Angle = aRenderable.LastAngle = rot.Angle;
    }
}

RenderLerpSystem::RenderLerpSystem()
{
}

RenderLerpSystem::~RenderLerpSystem()
{
}

void RenderLerpSystem::update(const float aAlpha)
{
    auto& r = getRegistry();
    auto& app = getApplication();

    auto now = std::chrono::high_resolution_clock::now();

    r.view<Renderable, Position>().each([aAlpha, now, &r](auto& renderable, auto& position) {
        if (position.Position != renderable.Position || now - renderable.LastPositionUpdate > std::chrono::milliseconds(25))
        {
            renderable.LastPositionUpdate = now;
            renderable.LastPosition = renderable.Position;
            renderable.Position = position.Position;
        }

        renderable.CurrentPosition = Util::GetLerped(aAlpha, renderable.LastPosition, renderable.Position);
    });
    r.view<Renderable, Rotation>().each([aAlpha, now, &r](auto& renderable, auto& angle) {
        if (angle.Angle != renderable.Angle || now - renderable.LastRotationUpdate > std::chrono::milliseconds(25))
        {
            renderable.LastRotationUpdate = now;
            renderable.LastAngle = renderable.Angle;
            renderable.Angle = angle.Angle;
        }

        // Use a spinor and spherical lerp to avoid issues when interpolating angles over the horizon
        renderable.CurrentAngle = Util::GetSlerpedAngle(aAlpha, renderable.LastAngle, renderable.Angle);
    });
}

void RenderLerpSystem::onInit()
{
    auto& r = getRegistry();

    r.on_construct<Renderable>().connect<onRenderableConstruct>();
}
