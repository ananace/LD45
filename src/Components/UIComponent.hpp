#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <entt/entity/entity.hpp>
#include <entt/entity/fwd.hpp>

namespace Components
{

struct UIComponent
{
    sf::FloatRect Position, CalculatedPosition;
    float ZIndex;

    entt::entity Parent;

    UIComponent() : ZIndex{}, Parent(entt::null) { }
};

}
