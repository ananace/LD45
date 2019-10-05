#include "RenderSystem.hpp"
#include "../Application.hpp"
#include "../Util.hpp"

#include "../Components/Position.hpp"
#include "../Components/Renderables.hpp"
#include "../Components/Rotation.hpp"

#include "entt/entity/utility.hpp"
#include "gsl/gsl_assert"
#include <SFML/Graphics/Drawable.hpp>

#include <gsl/gsl_util>

using Systems::RenderSystem;
using namespace Components;

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

    r.group<Renderable, const Rotation, const Position>().each([aAlpha, &r, &target](auto& renderable, auto& angle, auto& position) {
        if (position.Position != renderable.Position)
        {
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
    r.group<Renderable, const Position>(entt::exclude<Components::Rotation>).each([aAlpha, &r, &target](auto& renderable, auto& position) {
        if (position.Position != renderable.Position)
        {
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
