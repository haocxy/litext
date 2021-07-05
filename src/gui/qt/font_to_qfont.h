#pragma once

#include "core/font.h"
#include <QFont>


static void fontToQFont(const Font &font, QFont &qfont)
{
	qfont.setFamily(font.family().c_str());
	qfont.setPointSize(font.size());
	qfont.setBold(font.bold());
}

static QFont fontToQFont(const Font &font)
{
	QFont qfont;
	fontToQFont(font, qfont);
	return qfont;
}
