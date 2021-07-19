#include "row_walker.h"

#include "text/ustring_char_in_stream.h"
#include "text/txt_word_instream.h"
#include "text_area_config.h"


namespace gui
{

RowWalker::RowWalker(const TextAreaConfig &config, int widthLimit, CharInStream &charStream)
    : config_(config), widthLimit_(widthLimit), charStream_(charStream) {}

void RowWalker::forEachChar(CharOperation &&operation)
{
    const Pixel::Raw hGap = config_.hGap();
    const Pixel::Raw hMargin = config_.hMargin();

    TxtWordStream wordStream(charStream_);

    Pixel::Raw leftX = hGap;

    size_t currentLineIndex = 0;
    bool isCurrentLineEmpty = true;

    while (true) {
        const UString word = wordStream.Next();
        if (word.empty()) {
            break;
        }

        if (isCurrentLineEmpty) {
            for (const UChar c : word) {
                const Pixel::Raw charWidth = config_.charWidth(c);

                if (leftX + charWidth > widthLimit_) {
                    leftX = hGap;
                    ++currentLineIndex;
                    isCurrentLineEmpty = true;
                } else {
                    isCurrentLineEmpty = false;
                }

                operation(false, currentLineIndex, VChar(c, leftX, charWidth));

                leftX += charWidth;
                leftX += hMargin;
            }
        } else {
            Pixel::Raw wordWidth = 0;
            for (const UChar c : word) {
                wordWidth += config_.charWidth(c);
                wordWidth += hMargin;
            }
            if (leftX + wordWidth > widthLimit_) {
                leftX = hGap;
                ++currentLineIndex;
                isCurrentLineEmpty = true;
            } else {
                isCurrentLineEmpty = false;
            }
            for (const UChar c : word) {
                const Pixel::Raw charWidth = config_.charWidth(c);

                if (leftX + charWidth > widthLimit_) {
                    leftX = hGap;
                    ++currentLineIndex;
                    isCurrentLineEmpty = true;
                }

                operation(false, currentLineIndex, VChar(c, leftX, charWidth));

                leftX += charWidth;
                leftX += hMargin;
            }
        }
    }

    // 空行
    if (currentLineIndex == 0) {
        operation(true, 0, VChar());
    }
}

}
