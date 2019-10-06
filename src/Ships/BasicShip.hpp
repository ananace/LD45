#pragma once

#include <SFML/Graphics/ConvexShape.hpp>
#include <SFML/Graphics/Drawable.hpp>

namespace Ships
{

class BasicShip : public sf::Drawable
{
public:
    BasicShip();
    virtual void draw(sf::RenderTarget& aTarget, sf::RenderStates aStates) const override;

private:
    sf::ConvexShape m_shipShape,
                    m_cockpitShape,
                    m_greebleShape;
};

}
