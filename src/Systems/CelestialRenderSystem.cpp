#include "CelestialRenderSystem.hpp"
#include "../Application.hpp"
#include "../Util.hpp"

#include "../Components/Atmosphere.hpp"
#include "../Components/CelestialBody.hpp"
#include "../Components/PlanetShape.hpp"
#include "../Components/Renderables.hpp"
#include "../Components/SatteliteBody.hpp"
#include "../Components/StarField.hpp"
#include "../Components/StarShape.hpp"
#include "../Components/StationShape.hpp"

#include "gsl/gsl_assert"
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/VertexArray.hpp>

#include <array>
#include <chrono>
#include <gsl/gsl_util>

#include <random>
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

    std::random_device rDev;

    r.view<StarField>().each([&rDev, &target](auto& field) {
        if (GSL_UNLIKELY(field.CalculatedStars.empty()))
        {
            std::uniform_real_distribution<float> xDist(field.FieldSize.left, field.FieldSize.left + field.FieldSize.width);
            std::uniform_real_distribution<float> yDist(field.FieldSize.top, field.FieldSize.top + field.FieldSize.height);
            field.CalculatedStars.resize(field.Count);
            for (int i = 0; i < field.Count; ++i)
            {
                auto& vert = field.CalculatedStars[i];
                vert.position.x = xDist(rDev);
                vert.position.y = yDist(rDev);
            }
        }

        target.draw(field.CalculatedStars.data(), field.CalculatedStars.size(), sf::Points);
    });

    sf::Shader* atmosShader = m_atmosphereShader.get();
    sf::Shader* coronaShader = m_coronaShader.get();
    sf::CircleShape circle(64u);
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
    r.view<StarShape, Renderable>().each([coronaShader, &app, &target, &circle, aAlpha](auto& star, auto& lerp){
        lerp.CurrentPosition = Util::GetLerped(aAlpha, lerp.LastPosition, lerp.Position);

        const auto& size1 = target.getView().getSize();
        const auto& size2 = target.getSize();

        float scale = Util::GetLength({ size1.x / size2.x, size1.y / size2.y });
        float totalAlpha = std::chrono::duration<float>(app.getTotalTime()).count();

        auto coords = target.mapCoordsToPixel(lerp.CurrentPosition);
        coronaShader->setUniform("center", sf::Glsl::Vec4{ float(coords.x), size2.y - float(coords.y), star.Size / scale, (star.Size * 1.25f) / scale });
        coronaShader->setUniform("color", sf::Glsl::Vec4(star.CalculatedColor));
        coronaShader->setUniform("alpha", totalAlpha);

        circle.setFillColor(sf::Color::Transparent);
        circle.setRadius(star.Size * 3.f);
        circle.setOrigin(star.Size * 3.f, star.Size * 3.f);
        circle.setPosition(lerp.CurrentPosition);

        target.draw(circle, coronaShader);

        // circle.setFillColor(star.CalculatedColor);
        // circle.setRadius(star.Size);
        // circle.setOrigin(star.Size, star.Size);
        // circle.setPosition(lerp.CurrentPosition);

        // target.draw(circle);
    });
    r.view<PlanetShape, Renderable>().each([&target, &circle, aAlpha](auto& planet, auto& lerp){
        lerp.CurrentPosition = Util::GetLerped(aAlpha, lerp.LastPosition, lerp.Position);

        circle.setFillColor(planet.Color);
        circle.setRadius(planet.Size);
        circle.setOrigin(planet.Size, planet.Size);
        circle.setPosition(lerp.CurrentPosition);

        target.draw(circle);
    });

}

void CelestialRenderSystem::onInit()
{
    auto& app = getApplication();

    m_atmosphereShader = app.getResourceManager().load<sf::Shader>("Atmosphere");
    m_coronaShader = app.getResourceManager().load<sf::Shader>("Corona");
}
