#include "BasicShip.hpp"

#include <SFML/Graphics/ConvexShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

using namespace Ships;

BasicShip::BasicShip()
{
    m_shipShape.setPointCount(11);

    m_shipShape.setPoint(0, { 15, 0 });

    m_shipShape.setPoint(1, { 12, 3 });
    m_shipShape.setPoint(2, { 4, 3 });
    m_shipShape.setPoint(3, { -6, 12 });
    m_shipShape.setPoint(4, { -6, 2 });
    m_shipShape.setPoint(5, { -12, 2 });

    m_shipShape.setPoint(6, { -12, -2 });
    m_shipShape.setPoint(7, { -6, -2 });
    m_shipShape.setPoint(8, { -6, -12 });
    m_shipShape.setPoint(9, { 4, -3 });
    m_shipShape.setPoint(10, { 12, -3 });

    m_shipShape.setFillColor(sf::Color(196, 196, 196));
    m_shipShape.setOutlineColor(sf::Color::Black);
    m_shipShape.setOutlineThickness(1.f);

    m_cockpitShape.setPointCount(3);
    m_cockpitShape.setPoint(0, { 14.f, 0 });
    m_cockpitShape.setPoint(1, { 11.5f, 2.f });
    m_cockpitShape.setPoint(2, { 11.5f, -2.f });

    m_cockpitShape.setFillColor(sf::Color(0, 123, 167));
    m_cockpitShape.setOutlineColor(sf::Color(0, 0, 0, 64));
    m_cockpitShape.setOutlineThickness(0.5f);

    m_greebleShape.setPointCount(4);
    m_greebleShape.setPoint(0, { 11.4f, 2.f });
    m_greebleShape.setPoint(1, { -11.5f, 2.f });
    m_greebleShape.setPoint(2, { -11.5f, -2.f });
    m_greebleShape.setPoint(3, { 11.4f, -2.f });

    m_greebleShape.setFillColor(sf::Color(0, 0, 0, 64));
    m_greebleShape.setOutlineColor(sf::Color(0, 0, 0, 96));
    m_greebleShape.setOutlineThickness(0.5f);
}

void BasicShip::draw(sf::RenderTarget& aTarget, sf::RenderStates aStates) const
{
    aTarget.draw(m_shipShape, aStates);
    aTarget.draw(m_cockpitShape, aStates);
    aTarget.draw(m_greebleShape, aStates);
}
