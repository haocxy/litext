#include "row_walker.h"

#include "text/ustring_char_in_stream.h"
#include "text/txt_word_instream.h"
#include "text_area_config.h"


namespace gui
{

void NewRowWalker::forEachCharWithWrapLine(std::function<void(bool isEmptyRow, size_t lineIndex, const VChar &vchar)> &&action)
{
    const Pixel::Raw hGap = hGap_;
    const Pixel::Raw hPad = hPad_;

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
                const Pixel::Raw charWidth = charPixelWith(c);

                if (leftX + charWidth > widthLimit_) {
                    leftX = hGap;
                    ++currentLineIndex;
                    isCurrentLineEmpty = true;
                } else {
                    isCurrentLineEmpty = false;
                }

                action(false, currentLineIndex, VChar(c, leftX, charWidth));

                leftX += charWidth;
                leftX += hPad;
            }
        } else {
            Pixel::Raw wordWidth = 0;
            for (const UChar c : word) {
                wordWidth += charPixelWith(c);
                wordWidth += hPad;
            }
            if (leftX + wordWidth > widthLimit_) {
                leftX = hGap;
                ++currentLineIndex;
                isCurrentLineEmpty = true;
            } else {
                isCurrentLineEmpty = false;
            }
            for (const UChar c : word) {
                const Pixel::Raw charWidth = charPixelWith(c);

                if (leftX + charWidth > widthLimit_) {
                    leftX = hGap;
                    ++currentLineIndex;
                    isCurrentLineEmpty = true;
                }

                action(false, currentLineIndex, VChar(c, leftX, charWidth));

                leftX += charWidth;
                leftX += hPad;
            }
        }
    }

    // 空行
    if (currentLineIndex == 0) {
        action(true, 0, VChar());
    }
}

void NewRowWalker::forEachCharNoWrapLine(std::function<void(bool isEmptyRow, size_t lineIndex, const VChar &vchar)> &&action)
{
    throw std::logic_error("NewRowWalker::forEachCharNoWrapLine(...) unimplemented");
}

}
