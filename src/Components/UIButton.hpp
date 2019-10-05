#pragma once

#include "../Events/InputEvent.hpp"
#include "../Events/UIButtonClicked.hpp"

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Window/Event.hpp>
#include <entt/entity/fwd.hpp>

#include <string>

namespace Components
{

struct UIButton
{
    enum {
        kBevelSize = 5
    };
    enum : uint8_t{
        Flag_Hovered = 1 << 0,
        Flag_Pressed = 1 << 1,
    };

    std::string Text;
    sf::Color Color;
    uint8_t Flags;

    UIButton() : Flags{} { }
    UIButton(const std::string& aText) : Text(aText), Flags{} { }

    inline bool isHovered() const { return (Flags & Flag_Hovered) == Flag_Hovered; }
    inline bool isPressed() const { return (Flags & Flag_Pressed) == Flag_Pressed; }
};

}
