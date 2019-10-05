#pragma once

#include <entt/entity/fwd.hpp>

namespace Components
{

struct SatteliteBody
{
    entt::entity Orbiting;
    float Distance, Speed;

    float CurrentAngle;

    SatteliteBody() : Distance{}, Speed{}, CurrentAngle{} { }
    SatteliteBody(entt::entity aOrbiting, float aDistance, float aSpeed) : Orbiting(aOrbiting), Distance(aDistance), Speed(aSpeed), CurrentAngle{} { }
    SatteliteBody(entt::entity aOrbiting, float aDistance, float aSpeed, float aCurrentAngle) : Orbiting(aOrbiting), Distance(aDistance), Speed(aSpeed), CurrentAngle(aCurrentAngle) { }
};

}
