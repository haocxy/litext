#include "qt_util.h"

#include <QFont>
#include "util/font.h"

namespace QtUtil
{

void fillQFont(const view::Font &font, QFont &qfont)
{
	qfont.setFamily(font.family().c_str());
	qfont.setPointSize(font.size());
	qfont.setBold(font.bold());
}

} // namespace QtUtil
