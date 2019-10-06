#include "InputSystem.hpp"
#include "../Util.hpp"

#include "../Components/PlayerInput.hpp"
#include "../Components/Rotation.hpp"
#include "../Components/TradeJump.hpp"
#include "../Components/Velocity.hpp"

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

    auto inputs = r.group<Components::PlayerInput>(entt::get<Components::Velocity, Components::Rotation>);
    inputs.each([&inputVec, aDt](auto ent, auto& input, auto& phys, auto& angle){
        if (!input.Active)
            return;

        input.InputVector = inputVec;
        phys.Velocity += inputVec * 150.f * aDt;
        angle.Angle = Util::GetAngle(Util::GetNormalized(phys.Velocity));
    });
}

void InputSystem::onInit()
{

}
