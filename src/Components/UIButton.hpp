#pragma once

#include "../Events/InputEvent.hpp"
#include "../Events/UIButtonClicked.hpp"

#include <entt/signal/dispatcher.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Window/Event.hpp>

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

    UIButton() : Flags{}, Dispatcher{} {}
    UIButton(const std::string& aText) : Text(aText), Flags{}, Dispatcher{} {}

    std::string Text;
    sf::Color Color;
    sf::FloatRect Position;
    uint8_t Flags;
    entt::dispatcher* Dispatcher;

    inline bool isHovered() const { return (Flags & Flag_Hovered) == Flag_Hovered; }
    inline bool isPressed() const { return (Flags & Flag_Pressed) == Flag_Pressed; }

    inline void onMouseMove(const Events::InputEvent<sf::Event::MouseMoved>& ev) {
        bool hovered = Position.contains({ float(ev.Event.mouseMove.x), float(ev.Event.mouseMove.y) });

        if (hovered)
            Flags |= Flag_Hovered;
        else
            Flags &= ~Flag_Hovered;
    }
    inline void onMousePress(const Events::InputEvent<sf::Event::MouseButtonPressed>& ev) {
        if (ev.Event.mouseButton.button != sf::Mouse::Left)
            return;

        bool hovered = Position.contains({ float(ev.Event.mouseButton.x), float(ev.Event.mouseButton.y) });

        if (hovered)
            Flags |= Flag_Pressed;
        else
            Flags &= ~Flag_Pressed;
    }
    inline void onMouseRelease(const Events::InputEvent<sf::Event::MouseButtonReleased>& ev) {
        if (ev.Event.mouseButton.button != sf::Mouse::Left)
            return;

        bool hovered = Position.contains({ float(ev.Event.mouseButton.x), float(ev.Event.mouseButton.y) });

        if (hovered && (Flags & Flag_Pressed) == Flag_Pressed)
            Dispatcher->enqueue<Events::UIButtonClicked>({ Text });

        Flags &= ~Flag_Pressed;
    }
};

}
