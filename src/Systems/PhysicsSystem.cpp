#include "PhysicsSystem.hpp"
#include "../Util.hpp"

#include "../Components/Acceleration.hpp"
#include "../Components/DelayedAction.hpp"
#include "../Components/Friction.hpp"
#include "../Components/Position.hpp"
#include "../Components/TradeJump.hpp"
#include "../Components/Velocity.hpp"

#include <SFML/System/Vector2.hpp>
using Systems::PhysicsSystem;
using namespace Components;

void onTradeJumpDestroy(entt::entity aShip, entt::registry& aReg)
{
    // Add some residual trade speed

    bool hadVelocity = aReg.has<Velocity>(aShip);
    sf::Vector2f oldVelocity;
    if (hadVelocity)
        oldVelocity = aReg.get<Velocity>(aShip).Velocity;
    bool hadFriction = aReg.has<Velocity>(aShip);
    float oldFriction;
    if (hadFriction)
        oldFriction = aReg.get<Friction>(aShip).Friction;

    auto& tradeJump = aReg.get<const TradeJump>(aShip);
    auto direction = Util::GetNormalized(tradeJump.EndPosition - tradeJump.StartPosition);
    aReg.assign_or_replace<Velocity>(aShip, direction * 100.f);
    aReg.assign_or_replace<Friction>(aShip, 0.5f);

    aReg.assign<DelayedAction>(aShip, 0.75f, [&aReg, hadFriction, oldFriction, hadVelocity, oldVelocity](auto aShip) {
        if (hadVelocity)
            aReg.replace<Velocity>(aShip, oldVelocity);
        else
            aReg.remove<Velocity>(aShip);

        if (hadFriction)
            aReg.replace<Friction>(aShip, oldFriction);
        else
            aReg.remove<Velocity>(aShip);
    });
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

    r.group<TradeJump>(entt::get<Position>).each([&r, aDt](auto& tradeJump, auto& physical) {
        auto direction = Util::GetNormalized(tradeJump.EndPosition - tradeJump.StartPosition);
        tradeJump.Progress += aDt;

        physical.Position = Util::GetLerped(tradeJump.Progress / tradeJump.Time, tradeJump.StartPosition, tradeJump.EndPosition);
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
