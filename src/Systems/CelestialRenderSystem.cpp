#include "CelestialRenderSystem.hpp"
#include "../Application.hpp"
#include "../Math.hpp"
#include "../Util.hpp"

#include "../Components/Tags/TracedOrbit.hpp"
#include "../Components/Atmosphere.hpp"
#include "../Components/CelestialBody.hpp"
#include "../Components/GateShape.hpp"
#include "../Components/PlanetShape.hpp"
#include "../Components/Position.hpp"
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
    sf::Shader* coronaShader = m_coronaShader.get();
    sf::Shader* orbitShader = m_orbitShader.get();

    atmosShader->setUniform("alpha", totalAlpha);
    coronaShader->setUniform("alpha", totalAlpha);
    orbitShader->setUniform("alpha", totalAlpha);

    const auto& size1 = target.getView().getSize();
    const auto& size2 = target.getSize();
    const float scale = Util::GetLength({ size1.x / size2.x, size1.y / size2.y }) * 1.415f;

    atmosShader->setUniform("scale", scale);
    coronaShader->setUniform("scale", scale);
    orbitShader->setUniform("scale", scale);

    {
    sf::CircleShape orbitCircle(16u);
    orbitCircle.setFillColor(sf::Color::Transparent);
    r.group<const Tags::TracedOrbit>(entt::get<const SatteliteBody, const Position>).each([orbitShader, &size2, &r, &target, &orbitCircle](const auto& _orb, auto& body, auto& position){
        auto& orbitPos = r.get<const Position>(body.Orbiting).Position;

        auto coords = target.mapCoordsToPixel(orbitPos);
        orbitShader->setUniform("center", sf::Glsl::Vec4{ float(coords.x), size2.y - float(coords.y), body.Distance, 0.5f });

        orbitCircle.setPosition(orbitPos);
        orbitCircle.setRadius(body.Distance * 2.f);
        orbitCircle.setOrigin(body.Distance * 2.f, body.Distance * 2.f);

        target.draw(orbitCircle, orbitShader);
    });
    }

    sf::CircleShape circle(64u);
    r.group<const Atmosphere>(entt::get<Renderable>).each([atmosShader, &size2, &target, &circle, aAlpha](auto& atmos, auto& lerp){
        circle.setFillColor(sf::Color::Transparent);
        circle.setRadius(atmos.OuterSize * 2.f);
        circle.setOrigin(atmos.OuterSize * 2.f, atmos.OuterSize * 2.f);
        circle.setPosition(lerp.CurrentPosition);

        auto coords = target.mapCoordsToPixel(lerp.CurrentPosition);
        atmosShader->setUniform("center", sf::Glsl::Vec4{ float(coords.x), size2.y - float(coords.y), atmos.InnerSize, atmos.OuterSize });
        atmosShader->setUniform("color", sf::Glsl::Vec4(atmos.Color));

        target.draw(circle, atmosShader);
    });
    r.group<const StarShape>(entt::get<Renderable>).each([coronaShader, &size2, &target, &circle, aAlpha](auto& star, auto& lerp){
        auto coords = target.mapCoordsToPixel(lerp.CurrentPosition);
        coronaShader->setUniform("center", sf::Glsl::Vec4{ float(coords.x), size2.y - float(coords.y), star.Size, (star.Size * 1.25f) });
        coronaShader->setUniform("color", sf::Glsl::Vec4(star.CalculatedColor));

        circle.setFillColor(sf::Color::Transparent);
        circle.setRadius(star.Size * 3.f);
        circle.setOrigin(star.Size * 3.f, star.Size * 3.f);
        circle.setPosition(lerp.CurrentPosition);

        target.draw(circle, coronaShader);
    });
    r.group<const PlanetShape>(entt::get<Renderable>).each([&target, &circle, aAlpha](auto& planet, auto& lerp){
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
        gateShape.setPosition(lerp.CurrentPosition);
        gateShape.setRotation(gate.Angle * Math::RAD2DEG);

        target.draw(gateShape);
    });
}

void CelestialRenderSystem::onInit()
{
    auto& app = getApplication();

    m_atmosphereShader = app.getResourceManager().load<sf::Shader>("Atmosphere", true);
    m_coronaShader = app.getResourceManager().load<sf::Shader>("Corona", true);
    m_orbitShader = app.getResourceManager().load<sf::Shader>("Orbit", true);
}
