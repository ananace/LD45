#include "DebugView.hpp"
#include "Util.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Text.hpp>

void DebugView::startFrame()
{
}

void DebugView::endFrame()
{
}

void DebugView::draw(sf::RenderTarget& aTarget, sf::RenderStates states) const
{
    sf::Text debugString("=== [Debug] ===", Util::GetDefaultFont(Font_Monospace), 36u);

    aTarget.draw(debugString, states);
}
