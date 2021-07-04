#pragma once

#include <QFont>
#include <QFontMetrics>

#include "uchar.h"


struct FontInfo {
	std::string family;
	int size = 0;
	bool bold = false;
};

class Font {
public:
	Font() :metrics_(font_) {}

	void setFont(const FontInfo &fontInfo);

	std::string family() const { return font_.family().toStdString(); }

	int size() const { return font_.pointSize(); }

	bool bold() const { return font_.bold(); }

	int height() const { return metrics_.height(); }

	int ascent() const { return metrics_.ascent(); }
	
	int descent() const { return metrics_.descent(); }

	int charWidth(UChar unicode) const;

	bool isFixWidth() const;

private:
	QFont font_;
	QFontMetrics metrics_;
};
