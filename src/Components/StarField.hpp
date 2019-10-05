#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <vector>

namespace Components
{

struct StarField
{
    sf::FloatRect FieldSize;
    size_t Count;

    std::vector<sf::Vertex> CalculatedStars;

    StarField() : Count(1024) { }
};

}

