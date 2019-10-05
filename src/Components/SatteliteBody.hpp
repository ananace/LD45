#pragma once

#include <SFML/System/Vector2.hpp>
#include <entt/entity/fwd.hpp>

namespace Components
{

struct SatteliteBody
{
    entt::entity Orbiting;
    float Distance, Speed, Angle;

    sf::Vector2f CalculatedPosition;

    SatteliteBody() : Distance{}, Speed{}, Angle{} { }
};

}
