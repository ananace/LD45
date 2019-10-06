#pragma once

#include <cmath>

struct Spinor
{
    float Real, Complex;

    inline Spinor() : Real(0), Complex(0) { }
    inline Spinor(float aAngle) : Real(std::cos(aAngle)), Complex(std::sin(aAngle)) { }
    inline Spinor(float aReal, float aComplex) : Real(aReal), Complex(aComplex) { }

    inline float getAngle() const
    {
        return std::atan2(Complex, Real) * 2.f;
    }

    inline Spinor getSlerp(float aAlpha, const Spinor& aTo) const
    {
        float tc, tr, cosom = Real * aTo.Real + Complex * aTo.Complex;

        if (cosom < 0)
        {
            cosom = -cosom;
            tc = -aTo.Complex;
            tr = -aTo.Real;
        }
        else
        {
            tc = aTo.Complex;
            tr = aTo.Real;
        }

        float scale0, scale1;
        if (1 - cosom > 0.001f)
        {
            const float omega = std::acos(cosom);
            const float sinom = std::sin(omega);

            scale0 = std::sin((1 - aAlpha) * omega) / sinom;
            scale1 = std::sin(aAlpha * omega) / sinom;
        }
        else
        {
            scale0 = 1 - aAlpha;
            scale1 = aAlpha;
        }

        return Spinor(scale0 * Real + scale1 * tr, scale0 * Complex + scale1 * tc);
    }
};
