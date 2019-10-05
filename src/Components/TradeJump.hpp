#pragma once

#include <SFML/System/Vector2.hpp>
#include <entt/entity/fwd.hpp>

namespace Components
{

struct TradeJump
{
    sf::Vector2f TargetPosition;
    entt::entity TargetSystem;
};

}

