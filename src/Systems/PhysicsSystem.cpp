#include "PhysicsSystem.hpp"
#include "../Util.hpp"

#include "../Components/Tags/InSystem.hpp"
#include "../Components/Tags/JumpCapable.hpp"
#include "../Components/Acceleration.hpp"
#include "../Components/DelayedAction.hpp"
#include "../Components/Friction.hpp"
#include "../Components/JumpConnection.hpp"
#include "../Components/Position.hpp"
#include "../Components/TradeJump.hpp"
#include "../Components/Velocity.hpp"

#include <SFML/System/Vector2.hpp>
using Systems::PhysicsSystem;
using namespace Components;

void onTradeJumpCreate(entt::entity aShip, entt::registry& aReg, Components::TradeJump& tradeJump)
{
    bool hadVelocity = aReg.has<Velocity>(aShip);
    sf::Vector2f oldVelocity;
    if (hadVelocity)
        oldVelocity = aReg.get<Velocity>(aShip).Velocity;
    bool hadFriction = aReg.has<Velocity>(aShip);
    float oldFriction;
    if (hadFriction)
        oldFriction = aReg.get<Friction>(aShip).Friction;

    printf("[PhysicsSystem|D] Entity %d is entering trade jump to %d\n", int(aReg.entity(aShip)), int(aReg.entity(tradeJump.TargetSystem)));

    // Remove velocity and friction components during a trade jump
    aReg.remove<Velocity>(aShip);
    aReg.remove<Friction>(aShip);

    // aReg.assign<DelayedAction>(aShip, tradeJump.Time + 0.75f, [&aReg, hadFriction, oldFriction, hadVelocity, oldVelocity](auto aShip) {
    //     if (hadVelocity)
    //         aReg.replace<Velocity>(aShip, sf::Vector2f());
    //     else
    //         aReg.remove<Velocity>(aShip);

    //     if (hadFriction)
    //         aReg.replace<Friction>(aShip, oldFriction);
    //     else
    //         aReg.remove<Velocity>(aShip);
    // });
}

void onTradeJumpDestroy(entt::entity aShip, entt::registry& aReg)
{
    printf("[PhysicsSystem|D] Entity %d is finishing its trade jump\n", int(aReg.entity(aShip)));

    // Add some residual trade speed
    auto& tradeJump = aReg.get<Components::TradeJump>(aShip);
    auto direction = Util::GetNormalized(tradeJump.EndPosition - tradeJump.StartPosition);
    aReg.assign<Velocity>(aShip, direction * 100.f);
    aReg.assign<Friction>(aShip, 0.5f);
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

    r.group<TradeJump>(entt::get<Position>).each([&r, aDt](auto ent, auto& tradeJump, auto& physical) {
        auto direction = Util::GetNormalized(tradeJump.EndPosition - tradeJump.StartPosition);
        tradeJump.Progress += aDt;

        physical.Position = Util::GetLerped(tradeJump.Progress / tradeJump.Time, tradeJump.StartPosition, tradeJump.EndPosition);

        if (tradeJump.Progress >= tradeJump.Time * 1.0025f)
            r.remove<TradeJump>(ent);
    });

    r.view<const Position, const JumpConnection, const Tags::InSystem>().each([&r](auto& pos, auto& connection, auto& inSystem) {
        r.group<const Tags::JumpCapable>(entt::get<const Tags::InSystem, const Position>, entt::exclude<TradeJump>).each([&r, &pos, &inSystem, &connection](auto ent, const auto& _cap, const auto& entInSystem, const auto& entPos){
            if (entInSystem.System != inSystem.System)
                return;

            float distance = Util::GetLength(pos.Position - entPos.Position);
            if (distance < 10.f)
            {
                const auto& target = r.get<const Position>(connection.Target);
                const auto& targetSys = r.get<const Tags::InSystem>(connection.Target);

                r.assign<TradeJump>(ent, pos.Position, target.Position, connection.Time, targetSys.System);
            }
        });
    });

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
