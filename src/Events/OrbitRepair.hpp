#pragma once

#include <cstdint>

namespace Events
{

struct OrbitRepair
{
    uint8_t Rounds;

    OrbitRepair() : Rounds(1) {}
};

}
