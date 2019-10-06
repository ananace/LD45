#pragma once

#include <SFML/System/Vector2.hpp>
#include <entt/entity/fwd.hpp>

struct StarSystemGenerator
{
    void createSystem(entt::registry& aReg, const sf::Vector2f& aCenter = {});
    void createJumpGates(entt::registry& aReg);
    void createJumpHoles(entt::registry& aReg);
    void createColonies(entt::registry& aReg);
};
