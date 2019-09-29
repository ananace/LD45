#pragma once

#include <SFML/System/Vector2.hpp>

namespace sf { class Drawable; class Transformable; }

namespace Components
{

struct Renderable
{
    Renderable() : Angle{} {}

    sf::Vector2f Position;
    float Angle;

    sf::Drawable* Drawable;
};

struct LerpedRenderable
{
    sf::Drawable* Drawable;
    // sf::Transformable* Transformable;

    sf::Vector2f Position, LastPosition;
    float Angle, LastAngle;
};

}
