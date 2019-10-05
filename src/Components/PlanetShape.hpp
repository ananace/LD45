#pragma once

#include <SFML/Graphics/Color.hpp>

namespace Components
{

struct PlanetShape
{
    float Size;
    sf::Color Color;

    // enum Type;

    PlanetShape() : Size{} { }
    PlanetShape(float aSize) : Size(aSize) { }
    PlanetShape(float aSize, const sf::Color& aColor) : Size(aSize), Color(aColor) { }
};

}
