#pragma once

#include <SFML/Graphics/Color.hpp>

#include <random>
#include <cmath>
#include <cstdint>

namespace Components
{

struct StarShape
{
    enum StarType
    {
        Invalid,

        O,
        B,
        A,
        F,
        G,
        K,
        M
    };

    float Size;
    StarType Type;
    uint32_t Temperature;

    // Defaulting to sun-like
    StarShape(float aSize = 695.5f, StarType aType = StarType::G) : Size(aSize), Type(aType) {
        uint32_t lower = GetKelvin(Type);
        uint32_t upper = GetKelvin(StarType(Type - 1));

        std::random_device dev;
        Temperature = std::uniform_int_distribution<uint32_t>(lower, upper)(dev);

        CalculatedColor = GetColor(Temperature);
    }
    StarShape(float aSize, StarType aType, uint32_t aTemp) : Size(aSize), Type(aType), Temperature(aTemp) {
        CalculatedColor = GetColor(Temperature);
    }

    sf::Color CalculatedColor;

    static inline constexpr uint32_t GetKelvin(StarType aType) {
        switch (aType) {
        case StarType::Invalid:
            return 40000;
        case StarType::O:
            return 25000;
        case StarType::B:
            return 11000;
        case StarType::A:
            return 7500;
        case StarType::F:
            return 6000;
        case StarType::G:
            return 5000;
        case StarType::K:
            return 3500;
        case StarType::M:
        default:
            return 0;
        }
    };

    static inline sf::Color GetColor(uint32_t aKelvin) {
        sf::Color ret;

        auto temp = aKelvin / 100;

        if (temp <= 66)
            ret.r = 255;
        else
        {
            float red = temp - 60;
            red = 329.698727446 * std::pow(red, -0.1332047592);
            if (red < 0)
                ret.r = 0;
            else if (red > 255)
                ret.r = 255;
            else
                ret.r = uint8_t(red);
        }

        if (temp <= 66)
        {
            float green = temp;
            green = 99.4708025861 * std::log(green) - 161.1195681661;
            if (green < 0)
                ret.g = 0;
            else if (green > 255)
                ret.g = 255;
            else
                ret.g = uint8_t(green);
        }
        else
        {
            float green = temp - 60;
            green = 288.1221695283 * std::pow(green, -0.0755148492);
            if (green < 0)
                ret.g = 0;
            else if (green > 255)
                ret.g = 255;
            else
                ret.g = uint8_t(green);
        }

        if (temp >= 66)
            ret.b = 255;
        else
        {
            float blue = temp - 10;
            blue = 138.5177312231 * std::log(blue) - 305.0447927307;
            if (blue < 0)
                ret.b = 0;
            if (blue > 255)
                ret.b = 255;
            else
                ret.b = uint8_t(blue);
        }

        return ret;
    };
};

}
