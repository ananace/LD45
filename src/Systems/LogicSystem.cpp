#include "LogicSystem.hpp"
#include "../Util.hpp"

#include "../Components/DelayedAction.hpp"

#include <algorithm>
#include <gsl/gsl_assert>

using Systems::LogicSystem;

LogicSystem::LogicSystem()
{
}

LogicSystem::~LogicSystem()
{
}

void LogicSystem::update(const float aDt)
{
    auto& r = getRegistry();

    r.view<Components::DelayedAction>().each([&r, aDt](auto ent, auto& action) {
        float before = action.TimeRemaining;
        action.TimeRemaining -= aDt;

        if (before > 0 && action.TimeRemaining <= 0)
        {
            printf("[LogicSystem|D] Executing delayed action for %d\n", int(r.entity(ent)));
            action.Action(ent, r);

            r.remove<Components::DelayedAction>(ent);
        }
    });
}

void LogicSystem::onInit()
{

}
