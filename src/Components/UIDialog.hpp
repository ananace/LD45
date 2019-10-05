#pragma once

#include <SFML/Graphics/Color.hpp>

#include <string>

namespace Components
{

struct UIDialog
{
    enum {
        kBevelSize = 15
    };

    enum : uint8_t {
        Flag_Movable = 1 << 0,
    };

    std::string Title;
    sf::Color Color;
    uint8_t Flags;

    inline bool IsMovable() const { return (Flags & Flag_Movable) == Flag_Movable; }
};

}
