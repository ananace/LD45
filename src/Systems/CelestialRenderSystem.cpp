#include "CelestialRenderSystem.hpp"
#include "../Application.hpp"
#include "../Util.hpp"

#include "../Components/Atmosphere.hpp"
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

    r.view<CelestialBody, Renderable>().each([](auto& star, auto& lerp) {
        if (star.Position != lerp.Position)
        {
            lerp.LastPosition = lerp.Position;
            lerp.Position = star.Position;
        }
    });
    r.view<SatteliteBody, Renderable>().each([](auto& star, auto& lerp) {
        if (star.CalculatedPosition != lerp.Position)
        {
            lerp.LastPosition = lerp.Position;
            lerp.Position = star.CalculatedPosition;
        }
    });

    sf::Shader* atmosShader = m_atmosphereShader.get();
    sf::CircleShape circle;
    r.view<Atmosphere, Renderable>().each([atmosShader, &target, &circle, aAlpha](auto& atmos, auto& lerp){
        lerp.CurrentPosition = Util::GetLerped(aAlpha, lerp.LastPosition, lerp.Position);

        circle.setFillColor(sf::Color::Transparent);
        circle.setRadius(atmos.OuterSize);
        circle.setOrigin(atmos.OuterSize, atmos.OuterSize);
        circle.setPosition(lerp.CurrentPosition);

        const auto& size1 = target.getView().getSize();
        const auto& size2 = target.getSize();

        float scale = Util::GetLength({ size1.x / size2.x, size1.y / size2.y });

        auto coords = target.mapCoordsToPixel(lerp.CurrentPosition);
        atmosShader->setUniform("center", sf::Glsl::Vec4{ float(coords.x), size2.y - float(coords.y), atmos.InnerSize / scale, atmos.OuterSize / scale });
        atmosShader->setUniform("color", sf::Glsl::Vec4(atmos.Color));

        target.draw(circle, atmosShader);
    });
    r.view<StarShape, Renderable>().each([&target, &circle, aAlpha](auto& star, auto& lerp){
        lerp.CurrentPosition = Util::GetLerped(aAlpha, lerp.LastPosition, lerp.Position);

        circle.setFillColor(star.CalculatedColor);
        circle.setRadius(star.Size);
        circle.setOrigin(star.Size, star.Size);
        circle.setPosition(lerp.CurrentPosition);

        target.draw(circle);
    });
    r.view<PlanetShape, Renderable>().each([&target, &circle, aAlpha](auto& planet, auto& lerp){
        lerp.CurrentPosition = Util::GetLerped(aAlpha, lerp.LastPosition, lerp.Position);

        circle.setFillColor(planet.Color);
        circle.setRadius(planet.Size);
        circle.setOrigin(planet.Size, planet.Size);
        circle.setPosition(lerp.CurrentPosition);

        target.draw(circle);
    });

    r.view<PlanetShape, Renderable>();
}

void CelestialRenderSystem::onInit()
{
    auto& app = getApplication();

    m_atmosphereShader = app.getResourceManager().load<sf::Shader>("Atomsphere");
}
