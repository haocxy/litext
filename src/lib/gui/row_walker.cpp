#include "row_walker.h"

#include <stdexcept>

#include "text/txt_word_instream.h"


namespace gui
{

void RowWalker::forEachCharWithWrapLine(std::function<void(bool isEmptyRow, size_t lineIndex, const VChar &vchar)> &&action)
{
    TxtWordStream wordStream(charStream_);

    int leftX = hGap_;

    size_t currentLineIndex = 0;
    bool isCurrentLineEmpty = true;

    while (true) {
        const std::u32string word = wordStream.Next();
        if (word.empty()) {
            break;
        }

        if (isCurrentLineEmpty) {
            for (const char32_t c : word) {
                const int charWidth = charPixelWith(c);

                if (leftX + charWidth > widthLimit_) {
                    leftX = hGap_;
                    ++currentLineIndex;
                    isCurrentLineEmpty = true;
                } else {
                    isCurrentLineEmpty = false;
                }

                action(false, currentLineIndex, VChar(c, leftX, charWidth));

                leftX += charWidth;
                leftX += hPad_;
            }
        } else {
            int wordWidth = 0;
            for (const char32_t c : word) {
                wordWidth += charPixelWith(c);
                wordWidth += hPad_;
            }
            if (leftX + wordWidth > widthLimit_) {
                leftX = hGap_;
                ++currentLineIndex;
                isCurrentLineEmpty = true;
            } else {
                isCurrentLineEmpty = false;
            }
            for (const char32_t c : word) {
                const int charWidth = charPixelWith(c);

                if (leftX + charWidth > widthLimit_) {
                    leftX = hGap_;
                    ++currentLineIndex;
                    isCurrentLineEmpty = true;
                }

                action(false, currentLineIndex, VChar(c, leftX, charWidth));

                leftX += charWidth;
                leftX += hPad_;
            }
        }
    }

    // 空行
    if (currentLineIndex == 0) {
        action(true, 0, VChar());
    }
}

void RowWalker::forEachCharNoWrapLine(std::function<void(bool isEmptyRow, size_t lineIndex, const VChar &vchar)> &&action)
{
    throw std::logic_error("RowWalker::forEachCharNoWrapLine(...) unimplemented");
}

}
