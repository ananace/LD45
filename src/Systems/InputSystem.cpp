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

    auto inputs = r.group<Components::PlayerInput>(entt::get<Components::Velocity>);
    inputs.each([&inputVec, aDt](auto ent, auto& input, auto& phys){
        if (!input.Active)
            return;

        input.InputVector = inputVec;
        phys.Velocity += inputVec * 150.f * aDt;
    });

    const auto inputExtraEnd = inputs.raw<Components::PlayerInput>() + inputs.size<Components::PlayerInput>();
    const auto inputExtraBegin = inputs.raw<Components::PlayerInput>() + inputs.size();

    // Store input data even if no physical entity is connected
    std::for_each(inputExtraBegin, inputExtraEnd, [&inputVec](auto& input) {
        if (!input.Active)
            return;

        input.InputVector = inputVec;
    });
}

void InputSystem::onInit()
{

}
