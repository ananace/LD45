#include "CelestialRenderSystem.hpp"
#include "../Application.hpp"
#include "../Util.hpp"

#include "../Components/Atmosphere.hpp"
#include "../Components/CelestialBody.hpp"
#include "../Components/GateShape.hpp"
#include "../Components/PlanetShape.hpp"
#include "../Components/Renderables.hpp"
#include "../Components/SatteliteBody.hpp"
#include "../Components/StarField.hpp"
#include "../Components/StarShape.hpp"
#include "../Components/StationShape.hpp"

#include "gsl/gsl_assert"
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/ConvexShape.hpp>
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

    float totalAlpha = std::chrono::duration<float>(app.getTotalTime()).count();

    sf::Shader* atmosShader = m_atmosphereShader.get();
    atmosShader->setUniform("alpha", totalAlpha);
    sf::Shader* coronaShader = m_coronaShader.get();
    coronaShader->setUniform("alpha", totalAlpha);

    sf::CircleShape circle(64u);
    r.group<const Atmosphere>(entt::get<Renderable>).each([atmosShader, &target, &circle, aAlpha](auto& atmos, auto& lerp){
        lerp.CurrentPosition = Util::GetLerped(aAlpha, lerp.LastPosition, lerp.Position);

        circle.setFillColor(sf::Color::Transparent);
        circle.setRadius(atmos.OuterSize * 2.f);
        circle.setOrigin(atmos.OuterSize * 2.f, atmos.OuterSize * 2.f);
        circle.setPosition(lerp.CurrentPosition);

        const auto& size1 = target.getView().getSize();
        const auto& size2 = target.getSize();

        float scale = Util::GetLength({ size1.x / size2.x, size1.y / size2.y });

        auto coords = target.mapCoordsToPixel(lerp.CurrentPosition);
        atmosShader->setUniform("center", sf::Glsl::Vec4{ float(coords.x), size2.y - float(coords.y), atmos.InnerSize / scale, atmos.OuterSize / scale });
        atmosShader->setUniform("color", sf::Glsl::Vec4(atmos.Color));

        target.draw(circle, atmosShader);
    });
    r.group<const StarShape>(entt::get<Renderable>).each([coronaShader, &target, &circle, aAlpha](auto& star, auto& lerp){
        lerp.CurrentPosition = Util::GetLerped(aAlpha, lerp.LastPosition, lerp.Position);

        const auto& size1 = target.getView().getSize();
        const auto& size2 = target.getSize();

        float scale = Util::GetLength({ size1.x / size2.x, size1.y / size2.y });

        auto coords = target.mapCoordsToPixel(lerp.CurrentPosition);
        coronaShader->setUniform("center", sf::Glsl::Vec4{ float(coords.x), size2.y - float(coords.y), star.Size / scale, (star.Size * 1.25f) / scale });
        coronaShader->setUniform("color", sf::Glsl::Vec4(star.CalculatedColor));

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
    r.group<const PlanetShape>(entt::get<Renderable>).each([&target, &circle, aAlpha](auto& planet, auto& lerp){
        lerp.CurrentPosition = Util::GetLerped(aAlpha, lerp.LastPosition, lerp.Position);

        circle.setFillColor(planet.Color);
        circle.setRadius(planet.Size);
        circle.setOrigin(planet.Size, planet.Size);
        circle.setPosition(lerp.CurrentPosition);

        target.draw(circle);
    });

    sf::ConvexShape gateShape(5);
    gateShape.setPoint(0, { 10, 0 });
    gateShape.setPoint(1, { -10, 10 });
    gateShape.setPoint(2, { -3, 3.f });
    gateShape.setPoint(3, { -3, -3.f });
    gateShape.setPoint(4, { -10, -10 });

    gateShape.setFillColor(sf::Color::Transparent);
    gateShape.setOutlineColor(sf::Color(64, 196, 64));
    gateShape.setOutlineThickness(2.f);

    r.group<const GateShape>(entt::get<Renderable>).each([&target, &gateShape, aAlpha](auto& gate, auto& lerp){
        lerp.CurrentPosition = Util::GetLerped(aAlpha, lerp.LastPosition, lerp.Position);

        gateShape.setPosition(lerp.CurrentPosition);
        gateShape.setRotation(gate.Angle);

        target.draw(gateShape);
    });
}

void CelestialRenderSystem::onInit()
{
    auto& app = getApplication();

    m_atmosphereShader = app.getResourceManager().load<sf::Shader>("Atmosphere");
    m_coronaShader = app.getResourceManager().load<sf::Shader>("Corona");
}
