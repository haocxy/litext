#include "qt_util.h"

#include <QFont>
#include "core/font.h"


namespace QtUtil
{

void fillQFont(const Font &font, QFont &qfont)
{
	qfont.setFamily(font.family().c_str());
	qfont.setPointSize(font.size());
	qfont.setBold(font.bold());
}

} // namespace QtUtil
