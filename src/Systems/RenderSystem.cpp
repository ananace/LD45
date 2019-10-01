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

    for (auto& ent : r.view<LerpedRenderable>())
    {
        auto& renderable = r.get<LerpedRenderable>(ent);
        if (GSL_LIKELY(r.has<Physical>(ent)))
        {
            auto& phys = r.get<Physical>(ent);

            if (phys.Position != renderable.LastPosition)
            {
                renderable.LastPosition = phys.Position;
                renderable.Position = phys.Position + phys.Velocity * Application::kTickLength;
            }

            if (phys.Angle != renderable.Angle)
            {
                renderable.LastAngle = renderable.Angle;
                renderable.Angle = phys.Angle;
            }
        }

        auto pos = Util::GetLerped(aAlpha, renderable.LastPosition, renderable.Position);
        auto ang = Util::GetLerped(aAlpha, renderable.LastAngle, renderable.Angle);

        sf::RenderStates states;
        states.transform
            .translate(pos)
            .rotate(ang);

        target.draw(*renderable.Drawable, states);
    }
    for (auto& ent : r.view<Renderable>())
    {
        auto& renderable = r.get<Renderable>(ent);

        sf::RenderStates states;
        states.transform
            .translate(renderable.Position)
            .rotate(renderable.Angle);

        target.draw(*renderable.Drawable, states);
    }
}

