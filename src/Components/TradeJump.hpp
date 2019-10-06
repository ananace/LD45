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

    TradeJump(const sf::Vector2f& aStart, const sf::Vector2f& aEnd, float aTime, entt::entity aTargetSystem) : StartPosition(aStart), EndPosition(aEnd), Progress{}, Time(aTime), TargetSystem(aTargetSystem) { }
    TradeJump() : Progress{}, Time(5.f) { }
};

}

