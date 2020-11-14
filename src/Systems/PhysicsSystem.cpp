#include "PhysicsSystem.hpp"
#include "../Util.hpp"

#include "../Components/Tags/InSystem.hpp"
#include "../Components/Tags/JumpCapable.hpp"
#include "../Components/Acceleration.hpp"
#include "../Components/DelayedAction.hpp"
#include "../Components/Friction.hpp"
#include "../Components/JumpConnection.hpp"
#include "../Components/PlayerInput.hpp"
#include "../Components/Position.hpp"
#include "../Components/Rotation.hpp"
#include "../Components/TradeJump.hpp"
#include "../Components/Velocity.hpp"
#include "../Components/VisibleVelocity.hpp"

#include <SFML/System/Vector2.hpp>
using Systems::PhysicsSystem;
using namespace Components;

void onTradeJumpCreate(entt::registry& aReg, entt::entity aShip)
{
    bool hadFriction = aReg.has<Velocity>(aShip);
    float oldFriction = {};
    if (hadFriction)
        oldFriction = aReg.get<Friction>(aShip).Friction;

    auto& tradeJump = aReg.get<Components::TradeJump>(aShip);
    printf("[PhysicsSystem|D] Entity %d is entering trade jump to %d\n", int(aReg.entity(aShip)), int(aReg.entity(tradeJump.TargetSystem)));

    if (aReg.has<PlayerInput>(aShip))
        aReg.get<PlayerInput>(aShip).Active = false;

    // Remove velocity and friction components during a trade jump
    aReg.remove<Velocity>(aShip);
    aReg.remove<Friction>(aShip);

    auto direction = Util::GetNormalized(tradeJump.EndPosition - tradeJump.StartPosition);

    aReg.emplace_or_replace<VisibleVelocity>(aShip, 2.5f);
    aReg.emplace_or_replace<Rotation>(aShip, Util::GetAngle(direction));

    aReg.emplace<DelayedAction>(aShip, tradeJump.Time + 1.f, [hadFriction, oldFriction](entt::entity aShip, entt::registry& aReg) {
        if (hadFriction)
            aReg.replace<Friction>(aShip, oldFriction);
        else
            aReg.remove<Velocity>(aShip);
    });
}

void onTradeJumpDestroy(entt::registry& aReg, entt::entity aShip)
{
    printf("[PhysicsSystem|D] Entity %d is finishing its trade jump\n", int(aReg.entity(aShip)));

    if (aReg.has<PlayerInput>(aShip))
        aReg.get<PlayerInput>(aShip).Active = true;

    // Add some residual trade speed
    auto& tradeJump = aReg.get<Components::TradeJump>(aShip);
    auto direction = Util::GetNormalized(tradeJump.EndPosition - tradeJump.StartPosition);
    aReg.emplace<Velocity>(aShip, direction * 350.f);
    aReg.emplace<Friction>(aShip, 3.5f);

    aReg.remove<VisibleVelocity>(aShip);
}

PhysicsSystem::PhysicsSystem()
{
}

PhysicsSystem::~PhysicsSystem()
{
}

void PhysicsSystem::onInit()
{
    getRegistry().on_construct<TradeJump>().connect<onTradeJumpCreate>();
    getRegistry().on_destroy<TradeJump>().connect<onTradeJumpDestroy>();
}

void PhysicsSystem::update(const float aDt)
{
    auto& r = getRegistry();

    auto jumping = r.group<TradeJump>(entt::get<Position, Rotation>);
    for (auto jumpingEnt : jumping)
    {
        auto& tradeJump = jumping.get<TradeJump>(jumpingEnt);
        auto& physical = jumping.get<Position>(jumpingEnt);
        auto& rotation = jumping.get<Rotation>(jumpingEnt);
        auto direction = Util::GetNormalized(tradeJump.EndPosition - tradeJump.StartPosition);
        tradeJump.Progress += aDt;

        rotation.Angle = Util::GetAngle(direction);
        physical.Position = Util::GetLerped(tradeJump.Progress / tradeJump.Time, tradeJump.StartPosition, tradeJump.EndPosition);

        if (tradeJump.Progress >= tradeJump.Time * 1.0025f)
            r.remove<TradeJump>(jumpingEnt);
    };

    auto jumpCapable = r.group<Tags::JumpCapable>(entt::get<Tags::InSystem, Position>, entt::exclude<TradeJump>);
    r.view<Position, JumpConnection, Tags::InSystem>().each([&r, &jumpCapable](auto& pos, auto& connection, auto& inSystem) {
        for (auto jumpCapableEnt : jumpCapable)
        {
            auto& entInSystem = jumpCapable.get<Tags::InSystem>(jumpCapableEnt);
            if (entInSystem.System != inSystem.System)
                return;

            auto& entPos = jumpCapable.get<Position>(jumpCapableEnt);
            float distance = Util::GetLength(pos.Position - entPos.Position);
            if (distance < 10.f)
            {
                const auto& target = r.get<Position>(connection.Target);
                const auto& targetSys = r.get<Tags::InSystem>(connection.Target);

                r.emplace<TradeJump>(jumpCapableEnt, pos.Position, target.Position, connection.Time, targetSys.System);
            }
        };
    });

    auto friction = r.group<Friction>(entt::get<Velocity>);
    friction.each([aDt](auto& friction, auto& velocity) {
        velocity.Velocity -= velocity.Velocity * (friction.Friction * aDt);
    });
    auto acceleration = r.group<Acceleration>(entt::get<Velocity>);
    acceleration.each([aDt](auto& accel, auto& velocity) {
        velocity.Velocity += accel.Acceleration * aDt;
    });
    auto velocity = r.group<Velocity>(entt::get<Position>);
    velocity.each([aDt](auto& velocity, auto& position) {
        position.Position += velocity.Velocity * aDt;
    });
}
