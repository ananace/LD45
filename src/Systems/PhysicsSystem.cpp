#include "PhysicsSystem.hpp"

#include "../Components/Acceleration.hpp"
#include "../Components/Friction.hpp"
#include "../Components/Physical.hpp"

using Systems::PhysicsSystem;
using namespace Components;

PhysicsSystem::PhysicsSystem()
{
}

PhysicsSystem::~PhysicsSystem()
{
}

void PhysicsSystem::update(const float aDt)
{
    auto& r = getRegistry();

    r.view<Friction, Physical>().each([aDt](auto ent, auto& friction, auto& velocity) {
      velocity.Velocity -= velocity.Velocity * (friction.Friction * aDt);
    });
    r.view<Acceleration, Physical>().each([aDt](auto ent, auto& accel, auto& velocity) {
      velocity.Velocity += accel.Acceleration * aDt;
    });
    r.view<Physical>().each([aDt](auto ent, auto& physical) {
      physical.Position += physical.Velocity * aDt;
    });
}
