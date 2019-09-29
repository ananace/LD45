#pragma once

#include <SFML/Window/Event.hpp>

namespace Events
{

template<sf::Event::EventType Type>
struct InputEvent
{
    sf::Event Event;
};

}
