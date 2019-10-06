#pragma once

#include <SFML/System/Vector2.hpp>
#include <chrono>

namespace sf { class Drawable; class Transformable; }

namespace Components
{

struct Renderable
{
    Renderable() : Angle{}, LastAngle{}, CurrentAngle{} { }

    sf::Vector2f Position, LastPosition, CurrentPosition;
    float Angle, LastAngle, CurrentAngle;

    std::chrono::high_resolution_clock::time_point LastPositionUpdate, LastRotationUpdate;
};

struct DrawableRenderable
{
    sf::Drawable* Drawable;
};

}
