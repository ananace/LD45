#include "PhysicsSystem.hpp"
#include "../Util.hpp"

#include "../Components/Acceleration.hpp"
#include "../Components/Friction.hpp"
#include "../Components/Physical.hpp"
#include "../Components/TradeJump.hpp"

using Systems::PhysicsSystem;
using namespace Components;

void onTradeJumpDestroy(entt::entity aShip, entt::registry& aReg)
{
    auto physical = aReg.get<Physical>(aShip);

    // Drop the trade speed
    physical.Velocity = Util::GetWithLength(physical.Velocity, 300.f);
}

PhysicsSystem::PhysicsSystem()
{
}

PhysicsSystem::~PhysicsSystem()
{
}

void PhysicsSystem::onInit()
{
    getRegistry().on_destroy<TradeJump>().connect<onTradeJumpDestroy>();
}

void PhysicsSystem::update(const float aDt)
{
    auto& r = getRegistry();

    r.view<TradeJump, Physical>().each([aDt](auto& tradeJump, auto& physical) {
        auto direction = Util::GetNormalized(tradeJump.TargetPosition - physical.Position);
        physical.Velocity = Util::GetLerped(0.5f, physical.Velocity, direction * 3000.f);
    });

    r.view<Friction, Physical>().each([aDt](auto& friction, auto& velocity) {
        velocity.Velocity -= velocity.Velocity * (friction.Friction * aDt);
    });
    r.view<Acceleration, Physical>().each([aDt](auto& accel, auto& velocity) {
        velocity.Velocity += accel.Acceleration * aDt;
    });
    r.view<Physical>().each([aDt](auto& physical) {
        physical.Position += physical.Velocity * aDt;
    });
}
