#pragma once

#include <entt/entity/fwd.hpp>

namespace Components
{

struct JumpConnection
{
    entt::entity Target;
    float Time;
};

}
