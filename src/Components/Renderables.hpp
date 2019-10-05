#pragma once

#include <SFML/System/Vector2.hpp>

namespace sf { class Drawable; class Transformable; }

namespace Components
{

struct Renderable
{
    Renderable() : Angle{}, LastAngle{}, CurrentAngle{} { }

    sf::Vector2f Position, LastPosition, CurrentPosition;
    float Angle, LastAngle, CurrentAngle;
};

struct DrawableRenderable
{
    sf::Drawable* Drawable;
};

}
