#pragma once

#include "core/font_old.h"
#include <QFont>


static void fontToQFont(const FontOld &font, QFont &qfont)
{
	qfont.setFamily(font.family().c_str());
	qfont.setPointSize(font.size());
	qfont.setBold(font.bold());
}

static QFont fontToQFont(const FontOld &font)
{
	QFont qfont;
	fontToQFont(font, qfont);
	return qfont;
}
