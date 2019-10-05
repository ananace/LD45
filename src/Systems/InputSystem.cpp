#include "InputSystem.hpp"

#include "../Components/Physical.hpp"
#include "../Components/PlayerInput.hpp"

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

    r.view<Components::PlayerInput>().each([&r](auto ent, auto& input){
        if (!input.Active)
            return;

        if (GSL_LIKELY(r.has<Components::Physical>(ent)))
        {

        }
    });
}

void InputSystem::onInit()
{

}
