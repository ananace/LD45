#pragma once

#include <SFML/System/Vector2.hpp>

namespace Components
{

struct Physical
{
    sf::Vector2f Position, Velocity;
    float Angle;
};

}
