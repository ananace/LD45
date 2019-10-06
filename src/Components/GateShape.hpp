#pragma once

namespace Components
{

struct GateShape
{
    float Angle;

    // enum Type;

    GateShape(float aAngle) : Angle(aAngle) { }
    GateShape() : Angle{} { }
};

}
