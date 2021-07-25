#pragma once

#include <QFont>
#include <QFontMetrics>

#include "uchar.h"


struct FontInfoOld {
	std::string family;
	int size = 0;
	bool bold = false;
};

class FontOld {
public:
    FontOld():metrics_(font_) {}

	void setFont(const FontInfoOld &fontInfo);

	std::string family() const { return font_.family().toStdString(); }

	int size() const { return font_.pointSize(); }

	bool bold() const { return font_.bold(); }

	int height() const { return metrics_.height(); }

	int ascent() const { return metrics_.ascent(); }
	
	int descent() const { return metrics_.descent(); }

    int charWidth(UChar unicode) const {
        if (!UCharUtil::needSurrogate(unicode)) {
            return metrics_.horizontalAdvance(QChar(unicode));
        } else {
            QString pair;
            pair.push_back(QChar::highSurrogate(unicode));
            pair.push_back(QChar::lowSurrogate(unicode));
            return metrics_.horizontalAdvance(pair);
        }
    }

    bool isFixWidth() const {
        return isFixWidth_;
    }

private:
	QFont font_;
	QFontMetrics metrics_;
    bool isFixWidth_ = false;
};
