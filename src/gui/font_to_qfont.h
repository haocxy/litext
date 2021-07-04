#pragma once

#include "core/font.h"
#include <QFont>


void fontToQFont(const Font &font, QFont &qfont)
{
	qfont.setFamily(font.family().c_str());
	qfont.setPointSize(font.size());
	qfont.setBold(font.bold());
}

QFont fontToQFont(const Font &font)
{
	QFont qfont;
	fontToQFont(font, qfont);
	return qfont;
}
