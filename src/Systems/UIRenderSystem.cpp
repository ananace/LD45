#include "UIRenderSystem.hpp"
#include "../Application.hpp"
#include "../Util.hpp"

#include "../Components/UIButton.hpp"

#include <SFML/Graphics/ConvexShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Text.hpp>

using Systems::UIRenderSystem;
using namespace Components;

UIRenderSystem::UIRenderSystem()
{
}

UIRenderSystem::~UIRenderSystem()
{
}

void UIRenderSystem::onInit()
{
    auto& d = getDispatcher();

    d.sink<Events::InputEvent<sf::Event::MouseMoved>>().connect<&UIRenderSystem::onMouseMove>(*this);
    d.sink<Events::InputEvent<sf::Event::MouseButtonPressed>>().connect<&UIRenderSystem::onMousePress>(*this);
    d.sink<Events::InputEvent<sf::Event::MouseButtonReleased>>().connect<&UIRenderSystem::onMouseRelease>(*this);
}

void UIRenderSystem::update(const float aAlpha)
{
    auto& r = getRegistry();
    auto& app = getApplication();

    auto& target = app.getRenderWindow();

    /* The shape described looks like;
     *  1    2
     * 0/----\3
     *  |    |
     * 7\----/4
     *  6    5
     */
    sf::ConvexShape buttonShape(8);

    buttonShape.setFillColor({ 128, 255, 128 });
    buttonShape.setOutlineColor(sf::Color::Black);
    buttonShape.setOutlineThickness(2.f);

    const auto& font = Util::GetDefaultFont();

    sf::Text buttonText("[Placeholder]", font);
    buttonText.setFillColor(sf::Color::White);
    buttonText.setOutlineThickness(1.f);
    buttonText.setOutlineColor(sf::Color::Black);

    for (auto& ent : r.view<UIButton>())
    {
        auto& button = r.get<UIButton>(ent);

        buttonShape.setPoint(0, { -UIButton::kBevelSize, +UIButton::kBevelSize});
        buttonShape.setPoint(1, { +UIButton::kBevelSize, -UIButton::kBevelSize });
        buttonShape.setPoint(2, { +button.Position.width -UIButton::kBevelSize, -UIButton::kBevelSize });
        buttonShape.setPoint(3, { +button.Position.width +UIButton::kBevelSize, +UIButton::kBevelSize });
        buttonShape.setPoint(4, { +button.Position.width +UIButton::kBevelSize, +button.Position.height - UIButton::kBevelSize });
        buttonShape.setPoint(5, { +button.Position.width -UIButton::kBevelSize, +button.Position.height + UIButton::kBevelSize });
        buttonShape.setPoint(6, { +UIButton::kBevelSize, +button.Position.height + UIButton::kBevelSize });
        buttonShape.setPoint(7, { -UIButton::kBevelSize, +button.Position.height - UIButton::kBevelSize });

        auto color = button.Color;
        if (button.isPressed())
        {
            color.r *= 0.75;
            color.g *= 0.75;
            color.b *= 0.75;
        }
        else if (button.isHovered())
        {
            color.r *= 1.25;
            color.g *= 1.25;
            color.b *= 1.25;
        }
        buttonShape.setFillColor(color);

        buttonText.setString(button.Text);
        auto rect = buttonText.getLocalBounds();
        buttonText.setOrigin(rect.width / 2.f, rect.height / 2.f);

        float left = button.Position.left;
        if (left < 0)
            left += target.getView().getSize().x;
        float top = button.Position.top;
        if (top < 0)
            top += target.getView().getSize().y;

        sf::RenderStates states;
        states.transform
            .translate(left, top);

        if (button.isPressed())
            states.transform.translate(2, 2);

        target.draw(buttonShape, states);

        states.transform.translate(button.Position.width / 2.f, button.Position.height / 2.f);
        target.draw(buttonText, states);
    }
}

void UIRenderSystem::onMouseMove(const Events::InputEvent<sf::Event::MouseMoved>& ev)
{
    auto& r = getRegistry();
    sf::Vector2f mousePos{ float(ev.Event.mouseMove.x), float(ev.Event.mouseMove.y) };
    sf::Vector2f viewSize = getApplication().getRenderWindow().getView().getSize();

    r.view<UIButton>().each([&viewSize, &mousePos](auto& button) {
        auto rect = button.Position;
        if (rect.left < 0)
            rect.left += viewSize.x;
        if (rect.top < 0)
            rect.top += viewSize.y;

        bool hovered = rect.contains(mousePos);

        if (hovered)
            button.Flags |= UIButton::Flag_Hovered;
        else
            button.Flags &= ~UIButton::Flag_Hovered;
    });
}
void UIRenderSystem::onMousePress(const Events::InputEvent<sf::Event::MouseButtonPressed>& ev)
{
    if (ev.Event.mouseButton.button != sf::Mouse::Left)
        return;

    auto& r = getRegistry();
    sf::Vector2f mousePos{ float(ev.Event.mouseButton.x), float(ev.Event.mouseButton.y) };
    sf::Vector2f viewSize = getApplication().getRenderWindow().getView().getSize();

    r.view<UIButton>().each([&viewSize, &mousePos](auto& button) {
        auto rect = button.Position;
        if (rect.left < 0)
            rect.left += viewSize.x;
        if (rect.top < 0)
            rect.top += viewSize.y;
        bool hovered = rect.contains(mousePos);

        if (hovered)
            button.Flags |= UIButton::Flag_Pressed;
        else
            button.Flags &= ~UIButton::Flag_Pressed;
    });
}
void UIRenderSystem::onMouseRelease(const Events::InputEvent<sf::Event::MouseButtonReleased>& ev)
{
    if (ev.Event.mouseButton.button != sf::Mouse::Left)
        return;

    auto& r = getRegistry();
    auto& d = getDispatcher();
    sf::Vector2f mousePos{ float(ev.Event.mouseButton.x), float(ev.Event.mouseButton.y) };
    sf::Vector2f viewSize = getApplication().getRenderWindow().getView().getSize();

    r.view<UIButton>().each([&d, &viewSize, &mousePos](auto& button) {
        auto rect = button.Position;
        if (rect.left < 0)
            rect.left += viewSize.x;
        if (rect.top < 0)
            rect.top += viewSize.y;
        bool hovered = rect.contains(mousePos);

        if (hovered && (button.Flags & UIButton::Flag_Pressed) == UIButton::Flag_Pressed)
            d.enqueue<Events::UIButtonClicked>({ button.Text });

        button.Flags &= ~UIButton::Flag_Pressed;
    });
}
