#include "InputSystem.hpp"
#include "../Util.hpp"

#include "../Components/Velocity.hpp"
#include "../Components/PlayerInput.hpp"

#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <algorithm>
#include <gsl/gsl_assert>

using Systems::InputSystem;

InputSystem::InputSystem()
{
}

InputSystem::~InputSystem()
{
}

void InputSystem::update(const float aDt)
{
    auto& r = getRegistry();

    sf::Vector2f inputVec{
        float(sf::Keyboard::isKeyPressed(sf::Keyboard::D)) - sf::Keyboard::isKeyPressed(sf::Keyboard::A),
        float(sf::Keyboard::isKeyPressed(sf::Keyboard::S)) - sf::Keyboard::isKeyPressed(sf::Keyboard::W)
    };

    auto inputs = r.view<const Components::PlayerInput, Components::Velocity>();
    inputs.each([&inputVec, aDt](auto ent, auto& input, auto& phys){
        if (!input.Active)
            return;

        phys.Velocity += inputVec * 150.f * aDt;
    });

    // const auto inputExtraEnd = inputs.raw<const Components::PlayerInput>() + inputs.size<const Components::PlayerInput>();
    // const auto inputExtraBegin = inputs.raw<const Components::PlayerInput>() + inputs.size();

    // std::for_each(inputExtraBegin, inputExtraEnd, [](auto& input) {
    //     if (!input.Active)
    //         return;
    // });
}

void InputSystem::onInit()
{

}
