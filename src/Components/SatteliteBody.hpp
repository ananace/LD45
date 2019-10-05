#pragma once

#include <entt/entity/fwd.hpp>

namespace Components
{

struct SatteliteBody
{
    entt::entity Orbiting;
    float Distance, Speed, Angle;

    SatteliteBody() : Distance{}, Speed{}, Angle{} { }
};

}
