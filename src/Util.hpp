#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Font.hpp>
#include <gsl/gsl_util>
#include <iterator>
#include <cmath>

enum Font
{
    Font_Default,
    Font_Monospace,

    Font_COUNT
};

namespace Util
{
    inline float GetLength(const sf::Vector2f& aVec)
    {
        return std::sqrt(aVec.x * aVec.x + aVec.y * aVec.y);
    }

    inline sf::Vector2f GetNormalized(const sf::Vector2f& aVec)
    {
        if (GSL_UNLIKELY(aVec.x == 0 && aVec.y == 0))
            return aVec;

        float len = GetLength(aVec);
        return { aVec.x / len, aVec.y / len };
    }

    inline float GetDotProduct(const sf::Vector2f& aVec1, const sf::Vector2f& aVec2)
    {
        return aVec1.x * aVec2.x + aVec1.y * aVec2.y;
    }

    inline sf::Vector2f GetLerped(float aAlpha, const sf::Vector2f& aVecFrom, const sf::Vector2f& aVecTo)
    {
        return (1.f - aAlpha) * aVecFrom + aAlpha * aVecTo;
    }

    inline float GetLerped(float aAlpha, const float aFrom, const float aTo)
    {
        return (1.f - aAlpha) * aFrom + aAlpha * aTo;
    }

    inline sf::Font GetDefaultFont(Font type = Font_Default)
    {
        static struct DefaultFont
        {
            sf::Font Font;
            bool Loaded;
        } defaultFonts[Font_COUNT] = {};

        if (GSL_LIKELY(defaultFonts[type].Loaded))
        {
            return defaultFonts[type].Font;
        }

        std::array<std::string, 3> fonts;

        switch (type)
        {
        case Font_Monospace:
            fonts = {
#ifdef SFML_SYSTEM_LINUX
                "/usr/share/fonts/corefonts/andalemo.ttf",
                "/usr/share/fonts/dejavu/DejaVuSansMono.ttf",
                "/usr/share/fonts/liberation-fonts/LiberationMono-Regular.ttf"
#elif defined(SFML_SYSTEM_WINDOWS)
                "C:\\Windows\\Fonts\\consola.ttf",
                "C:\\Windows\\Fonts\\segoeui.ttf",
                "C:\\Windows\\Fonts\\cour.ttf"
#endif
            }; break;

        default:
            fonts = {
#ifdef SFML_SYSTEM_LINUX
                "/usr/share/fonts/corefonts/verdana.ttf",
                "/usr/share/fonts/dejavu/DejaVuSans.ttf",
                "/usr/share/fonts/liberation-fonts/LiberationSans-Regular.ttf"
#elif defined(SFML_SYSTEM_WINDOWS)
                "C:\\Windows\\Fonts\\verdana.ttf",
                "C:\\Windows\\Fonts\\trebuc.ttf",
                "C:\\Windows\\Fonts\\arial.ttf"
#endif
            }; break;
        }

        auto& font = defaultFonts[type].Font;
        for (auto& path : fonts)
            if (font.loadFromFile(path))
            {
                defaultFonts[type].Loaded = true;
                break;
            }

        if (GSL_UNLIKELY(!defaultFonts[type].Loaded))
            throw std::runtime_error("Failed to load a valid font file");

        return font;
    }

    template <typename T>
    struct reversion_wrapper { T& iterable; };

    template <typename T>
    auto begin (reversion_wrapper<T> w) { return std::rbegin(w.iterable); }

    template <typename T>
    auto end (reversion_wrapper<T> w) { return std::rend(w.iterable); }

    template <typename T>
    reversion_wrapper<T> reverse (T&& iterable) { return { iterable }; }
}
