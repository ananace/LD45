#include "RenderSystem.hpp"
#include "../Application.hpp"
#include "../Util.hpp"

#include "../Components/Physical.hpp"
#include "../Components/Renderables.hpp"

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

    r.view<Physical, Renderable>().each([aAlpha, &r, &target](auto ent, auto& phys, auto& renderable) {
        if (phys.Position != renderable.Position)
        {
            renderable.LastPosition = renderable.Position;
            renderable.Position = phys.Position;
        }

        if (phys.Angle != renderable.Angle)
        {
            renderable.LastAngle = renderable.Angle;
            renderable.Angle = phys.Angle;
        }

        renderable.CurrentPosition = Util::GetLerped(aAlpha, renderable.LastPosition, renderable.Position);
        renderable.CurrentAngle = Util::GetLerped(aAlpha, renderable.LastAngle, renderable.Angle);

        sf::RenderStates states;
        states.transform
            .translate(renderable.CurrentPosition)
            .rotate(renderable.CurrentAngle);

        if (r.has<DrawableRenderable>(ent))
            target.draw(*r.get<DrawableRenderable>(ent).Drawable, states);
    });
}

