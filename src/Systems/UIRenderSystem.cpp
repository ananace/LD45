#include "UIRenderSystem.hpp"
#include "../Application.hpp"
#include "../Util.hpp"

#include "../Components/UIButton.hpp"
#include "../Components/UIComponent.hpp"
#include "../Components/UIDialog.hpp"

#include <SFML/Graphics/ConvexShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Text.hpp>

#include <limits>
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

    // UI Preparation
    sf::Vector2f viewSize = getApplication().getRenderWindow().getView().getSize();
    for (int i = 0; i < 2; ++i)
        r.view<UIComponent>().each([&viewSize, &r](auto& comp) {
            auto rect = comp.Position;
            if (r.valid(comp.Parent))
            {
                auto& parentComponent = r.get<UIComponent>(comp.Parent);

                if (rect.left < 0)
                    rect.left += parentComponent.CalculatedPosition.width;
                if (rect.top < 0)
                    rect.top += parentComponent.CalculatedPosition.height;

                rect.left += parentComponent.CalculatedPosition.left;
                rect.top += parentComponent.CalculatedPosition.top;
            }
            else
            {
                if (rect.left < 0)
                    rect.left += viewSize.x;
                if (rect.top < 0)
                    rect.top += viewSize.y;
            }
            comp.CalculatedPosition = rect;

            if (!r.valid(comp.Parent))
                return;

            comp.ZIndex = r.get<UIComponent>(comp.Parent).ZIndex + std::numeric_limits<float>::epsilon();
        });

    r.sort<UIComponent>([](const auto& lhs, const auto& rhs) {
        return lhs.ZIndex < rhs.ZIndex;
    });

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

    auto view = r.view<UIComponent>();
    for (auto& ent : view)
    {
        auto& component = r.get<UIComponent>(ent);

        sf::RenderStates states;
        states.transform
            .translate(component.CalculatedPosition.left, component.CalculatedPosition.top);

        if (r.has<UIDialog>(ent))
        {
            auto& dialog = r.get<UIDialog>(ent);

            buttonShape.setPoint(0, { -UIDialog::kBevelSize, +UIDialog::kBevelSize});
            buttonShape.setPoint(1, { +UIDialog::kBevelSize, -UIDialog::kBevelSize });
            buttonShape.setPoint(2, { +component.Position.width -UIDialog::kBevelSize, -UIDialog::kBevelSize });
            buttonShape.setPoint(3, { +component.Position.width +UIDialog::kBevelSize, +UIDialog::kBevelSize });
            buttonShape.setPoint(4, { +component.Position.width +UIDialog::kBevelSize, +component.Position.height - UIDialog::kBevelSize });
            buttonShape.setPoint(5, { +component.Position.width -UIDialog::kBevelSize, +component.Position.height + UIDialog::kBevelSize });
            buttonShape.setPoint(6, { +UIDialog::kBevelSize, +component.Position.height + UIDialog::kBevelSize });
            buttonShape.setPoint(7, { -UIDialog::kBevelSize, +component.Position.height - UIDialog::kBevelSize });

            buttonShape.setFillColor(dialog.Color);

            target.draw(buttonShape, states);
        }
        else if (r.has<UIButton>(ent))
        {
            auto& button = r.get<UIButton>(ent);

            buttonShape.setPoint(0, { -UIButton::kBevelSize, +UIButton::kBevelSize});
            buttonShape.setPoint(1, { +UIButton::kBevelSize, -UIButton::kBevelSize });
            buttonShape.setPoint(2, { +component.Position.width -UIButton::kBevelSize, -UIButton::kBevelSize });
            buttonShape.setPoint(3, { +component.Position.width +UIButton::kBevelSize, +UIButton::kBevelSize });
            buttonShape.setPoint(4, { +component.Position.width +UIButton::kBevelSize, +component.Position.height - UIButton::kBevelSize });
            buttonShape.setPoint(5, { +component.Position.width -UIButton::kBevelSize, +component.Position.height + UIButton::kBevelSize });
            buttonShape.setPoint(6, { +UIButton::kBevelSize, +component.Position.height + UIButton::kBevelSize });
            buttonShape.setPoint(7, { -UIButton::kBevelSize, +component.Position.height - UIButton::kBevelSize });

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

            if (button.isPressed())
                states.transform.translate(2, 2);

            target.draw(buttonShape, states);

            states.transform.translate(component.Position.width / 2.f, component.Position.height / 2.f);
            target.draw(buttonText, states);
        }
    }
}

void UIRenderSystem::onMouseMove(const Events::InputEvent<sf::Event::MouseMoved>& ev)
{
    auto& r = getRegistry();
    sf::Vector2f mousePos{ float(ev.Event.mouseMove.x), float(ev.Event.mouseMove.y) };
    sf::Vector2f viewSize = getApplication().getRenderWindow().getView().getSize();

    r.group<const UIComponent, UIButton>().each([&viewSize, &mousePos](auto& comp, auto& button) {
        auto& rect = comp.CalculatedPosition;
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

    r.group<const UIComponent, UIButton>().each([&viewSize, &mousePos](auto& comp, auto& button) {
        auto& rect = comp.CalculatedPosition;
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

    r.group<const UIComponent, UIButton>().each([&d, &viewSize, &mousePos](auto& comp, auto& button) {
        auto& rect = comp.CalculatedPosition;
        bool hovered = rect.contains(mousePos);

        if (hovered && (button.Flags & UIButton::Flag_Pressed) == UIButton::Flag_Pressed)
            d.enqueue<Events::UIButtonClicked>({ button.Text });

        button.Flags &= ~UIButton::Flag_Pressed;
    });
}
