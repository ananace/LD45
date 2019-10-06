#include "RenderSystem.hpp"
#include "../Application.hpp"
#include "../Math.hpp"
#include "../Util.hpp"

#include "../Components/Position.hpp"
#include "../Components/Renderables.hpp"
#include "../Components/Rotation.hpp"
#include "../Components/VisibleVelocity.hpp"

#include <SFML/Graphics/Drawable.hpp>

#include <entt/entity/fwd.hpp>
#include <gsl/gsl_assert>

#include <chrono>
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

    r.group<const DrawableRenderable>(entt::get<const Renderable, const VisibleVelocity>).each([&target](auto& drawable, auto& renderable, auto& velocity) {
        sf::RenderStates states;
        states.transform
            .translate(renderable.CurrentPosition)
            .rotate(renderable.CurrentAngle * Math::RAD2DEG)
            .scale(1.f + velocity.Velocity / 2.f, 1.f - velocity.Velocity / 10.f);

        target.draw(*drawable.Drawable, states);
    });

    r.group<const DrawableRenderable>(entt::get<const Renderable>, entt::exclude<VisibleVelocity>).each([&target](auto& drawable, auto& renderable) {
        sf::RenderStates states;
        states.transform
            .translate(renderable.CurrentPosition)
            .rotate(renderable.CurrentAngle * Math::RAD2DEG);

        target.draw(*drawable.Drawable, states);
    });
}
