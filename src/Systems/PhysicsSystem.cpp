#include "PhysicsSystem.hpp"
#include "../Util.hpp"

#include "../Components/Acceleration.hpp"
#include "../Components/Friction.hpp"
#include "../Components/Position.hpp"
#include "../Components/TradeJump.hpp"
#include "../Components/Velocity.hpp"

using Systems::PhysicsSystem;
using namespace Components;

void onTradeJumpDestroy(entt::entity aShip, entt::registry& aReg)
{
    auto velocity = aReg.get<Velocity>(aShip);

    // Drop the trade speed
    velocity.Velocity = Util::GetWithLength(velocity.Velocity, 300.f);
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

    r.group<const TradeJump>(entt::get<const Position, Velocity>).each([&r, aDt](auto& tradeJump, auto& physical, auto& velocity) {
        auto direction = Util::GetNormalized(tradeJump.TargetPosition - physical.Position);
        velocity.Velocity = Util::GetLerped(0.5f, velocity.Velocity, direction * 3000.f);
    });
    // r.group<const TradeJump, const Position>(entt::exclude<Components::Velocity>).each([&r, aDt](auto ent, auto& tradeJump, auto& physical) {
    //     auto direction = Util::GetNormalized(tradeJump.TargetPosition - physical.Position);
    //     r.assign<Components::Velocity>(ent, direction * 3000.f);
    // });

    r.group<const Friction>(entt::get<Velocity>).each([aDt](auto& friction, auto& velocity) {
        velocity.Velocity -= velocity.Velocity * (friction.Friction * aDt);
    });
    r.group<const Acceleration>(entt::get<Velocity>).each([aDt](auto& accel, auto& velocity) {
        velocity.Velocity += accel.Acceleration * aDt;
    });
    r.group<const Velocity>(entt::get<Position>).each([aDt](auto& velocity, auto& position) {
        position.Position += velocity.Velocity * aDt;
    });
}
