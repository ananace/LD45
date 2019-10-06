#include "RenderSystem.hpp"
#include "../Application.hpp"
#include "../Util.hpp"

#include "../Components/Position.hpp"
#include "../Components/Renderables.hpp"
#include "../Components/Rotation.hpp"

#include "entt/entity/fwd.hpp"
#include "gsl/gsl_assert"
#include <SFML/Graphics/Drawable.hpp>

#include <chrono>
#include <gsl/gsl_util>

using Systems::RenderSystem;
using namespace Components;

void onRenderableConstruct(entt::entity aEnt, entt::registry& aRegistry, Renderable& aRenderable)
{
    if (!aRegistry.has<Position>(aEnt))
        return;

    auto& pos = aRegistry.get<const Position>(aEnt);
    aRenderable.Position = aRenderable.LastPosition = pos.Position;
}

RenderSystem::RenderSystem()
{
}

RenderSystem::~RenderSystem()
{
}

void RenderSystem::update(const float aAlpha)
{
    auto& r = getRegistry();
    auto& app = getApplication();

    auto& target = app.getRenderWindow();
    auto now = std::chrono::high_resolution_clock::now();

    r.group<Renderable, const Rotation, const Position>().each([aAlpha, now, &r, &target](auto& renderable, auto& angle, auto& position) {
        if (position.Position != renderable.Position || now - renderable.LastUpdate > std::chrono::milliseconds(25))
        {
            renderable.LastUpdate = now;
            renderable.LastPosition = renderable.Position;
            renderable.Position = position.Position;
        }
        if (angle.Angle != renderable.Angle)
        {
            renderable.LastAngle = renderable.Angle;
            renderable.Angle = angle.Angle;
        }

        renderable.CurrentPosition = Util::GetLerped(aAlpha, renderable.LastPosition, renderable.Position);
        renderable.CurrentAngle = Util::GetLerped(aAlpha, renderable.LastAngle, renderable.Angle);
    });
    r.group<Renderable, const Position>(entt::exclude<Components::Rotation>).each([aAlpha, now, &r, &target](auto& renderable, auto& position) {
        if (position.Position != renderable.Position || now - renderable.LastUpdate > std::chrono::milliseconds(25))
        {
            renderable.LastUpdate = now;
            renderable.LastPosition = renderable.Position;
            renderable.Position = position.Position;
        }

        renderable.CurrentPosition = Util::GetLerped(aAlpha, renderable.LastPosition, renderable.Position);
    });
    r.group<Renderable, const Rotation>(entt::exclude<Components::Position>).each([aAlpha, &r, &target](auto& renderable, auto& angle) {
        if (angle.Angle != renderable.Angle)
        {
            renderable.LastAngle = renderable.Angle;
            renderable.Angle = angle.Angle;
        }

        renderable.CurrentAngle = Util::GetLerped(aAlpha, renderable.LastAngle, renderable.Angle);
    });

    r.group<const DrawableRenderable>(entt::get<const Renderable>).each([&target](auto& drawable, auto& renderable) {
        if (GSL_UNLIKELY(!drawable.Drawable))
            return;

        sf::RenderStates states;
        states.transform.translate(renderable.CurrentPosition).rotate(renderable.CurrentAngle);

        target.draw(*drawable.Drawable, states);
    });
}

void RenderSystem::onInit()
{
    auto& r = getRegistry();

    r.on_construct<Renderable>().connect<onRenderableConstruct>();
}
