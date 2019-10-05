#pragma once

#include <string>

namespace sf { class Drawable; }

namespace Components
{

struct StationShape
{
    float Size;
    std::string DrawableName;

    sf::Drawable* Drawable;

    StationShape() : Size{}, Drawable{} { }
};

}
