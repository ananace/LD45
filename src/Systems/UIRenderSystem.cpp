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

        sf::RenderStates states;
        states.transform
            .translate(button.Position.left, button.Position.top);

        if (button.isPressed())
            states.transform.translate(2, 2);

        target.draw(buttonShape, states);

        states.transform.translate(buttonText.getLocalBounds().width / 4.f, -buttonText.getLocalBounds().height / 4.f);
        target.draw(buttonText, states);
    }
}

