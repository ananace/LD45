#pragma once

#include <SFML/Graphics/Drawable.hpp>
#include <chrono>

namespace sf { class RenderTarget; }

class DebugView : public sf::Drawable
{
public:
    void startFrame();
    void endFrame();

    void draw(sf::RenderTarget& aTarget, sf::RenderStates states) const;

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> m_lastDraw;
};
