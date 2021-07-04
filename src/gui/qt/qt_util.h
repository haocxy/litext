#pragma once

namespace view
{
class Font;
}

class QFont;

namespace QtUtil
{

void fillQFont(const view::Font &font, QFont &qfont);

} // namespace QtUtil
