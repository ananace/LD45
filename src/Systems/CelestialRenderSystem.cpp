#include "CelestialRenderSystem.hpp"
#include "../Application.hpp"
#include "../Util.hpp"

#include "../Components/CelestialBody.hpp"
#include "../Components/PlanetShape.hpp"
#include "../Components/Renderables.hpp"
#include "../Components/SatteliteBody.hpp"
#include "../Components/StarShape.hpp"
#include "../Components/StationShape.hpp"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Shader.hpp>

#include <gsl/gsl_util>

using Systems::CelestialRenderSystem;
using namespace Components;

CelestialRenderSystem::CelestialRenderSystem()
{
}

CelestialRenderSystem::~CelestialRenderSystem()
{
}

void CelestialRenderSystem::update(const float aAlpha)
{
    auto& r = getRegistry();
    auto& app = getApplication();

    auto& target = app.getRenderWindow();

    r.view<CelestialBody, LerpedDirectRenderable>().each([](auto ent, auto& star, auto& lerp) {
        if (star.Position != lerp.Position)
        {
            lerp.LastPosition = lerp.Position;
            lerp.Position = star.Position;
        }
    });
    r.view<SatteliteBody, LerpedDirectRenderable>().each([](auto ent, auto& star, auto& lerp) {
        if (star.CalculatedPosition != lerp.Position)
        {
            lerp.LastPosition = lerp.Position;
            lerp.Position = star.CalculatedPosition;
        }
    });

    sf::CircleShape circle;
    r.view<StarShape, LerpedDirectRenderable>().each([&target, &circle, aAlpha](auto ent, auto& star, auto& lerp){
        auto pos = Util::GetLerped(aAlpha, lerp.LastPosition, lerp.Position);

        circle.setFillColor(star.CalculatedColor);
        circle.setRadius(star.Size);
        circle.setOrigin(star.Size, star.Size);
        circle.setPosition(pos);

        target.draw(circle);
    });

    r.view<PlanetShape, LerpedDirectRenderable>();
}

void CelestialRenderSystem::onInit()
{
    auto& app = getApplication();

    m_atomsphereShader = app.getResourceManager().load<sf::Shader>("Atomsphere");
}
