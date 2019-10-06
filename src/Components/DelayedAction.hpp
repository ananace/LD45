#pragma once

#include <entt/entity/fwd.hpp>
#include <functional>

namespace Components
{

struct DelayedAction
{
    float TimeRemaining;
    std::function<void(entt::entity, entt::registry&)> Action;
};

}
