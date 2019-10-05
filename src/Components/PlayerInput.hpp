#pragma once

#include <SFML/System/Vector2.hpp>

namespace Components
{

struct PlayerInput
{
    bool Active;
    sf::Vector2f InputVector;

    PlayerInput() : Active(true) {}
};

}
