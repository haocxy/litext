#pragma once


class Font;
class QFont;

namespace gui::qt
{

void fillQFont(const Font &font, QFont &qfont);

} // namespace QtUtil
