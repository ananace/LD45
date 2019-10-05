#pragma once

#include <SFML/System/Vector2.hpp>
#include <entt/entity/fwd.hpp>

namespace Components
{

struct TradeJump
{
    sf::Vector2f StartPosition, EndPosition;
    float Progress, Time;

    entt::entity TargetSystem;

    TradeJump() : Progress{}, Time(5.f) { }
};

}

