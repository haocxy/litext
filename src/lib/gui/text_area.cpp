#include "text_area.h"

#include "text_area_impl.h"


namespace gui
{

TextArea::TextArea(Editor &editor, const TextAreaConfig &config)
    : impl_(new impl::TextArea(editor, config))
{
}

TextArea::~TextArea()
{
    delete impl_;
    impl_ = nullptr;
}

const doc::Document &TextArea::doc() const
{
    return impl_->doc();
}

doc::Document &TextArea::doc()
{
    return impl_->doc();
}

void TextArea::open(const Size &size, RowN row)
{
    impl_->open(size, row);
}

void TextArea::resize(const Size &size)
{
    impl_->resize(size);
}

void TextArea::jumpTo(RowN row)
{
    impl_->jumpTo(row);
}

RowN TextArea::rowOffset() const
{
    return impl_->rowOffset();
}

LineN TextArea::lineCountLimit() const
{
    return impl_->lineCountLimit();
}

void TextArea::onPrimaryButtomPress(i32 x, i32 y)
{
    impl_->onPrimaryButtomPress(x, y);
}

void TextArea::onDirUpKeyPress()
{
    impl_->onDirUpKeyPress();
}

void TextArea::onDirDownKeyPress()
{
    impl_->onDirDownKeyPress();
}

void TextArea::onDirLeftKeyPress()
{
    impl_->onDirLeftKeyPress();
}

void TextArea::onDirRightKeyPress()
{
    impl_->onDirRightKeyPress();
}

bool TextArea::moveDownByOneLine()
{
    return impl_->moveDownByOneLine();
}

int TextArea::width() const
{
    return impl_->width();
}

int TextArea::height() const
{
    return impl_->height();
}

Editor &TextArea::editor()
{
    return impl_->editor();
}

const TextAreaConfig &TextArea::config() const
{
    return impl_->config();
}

std::optional<Rect> TextArea::getLastActLineDrawRect() const
{
    return impl_->getLastActLineDrawRect();
}

std::optional<VerticalLine> TextArea::getNormalCursorDrawData() const
{
    return impl_->getNormalCursorDrawData();
}

int TextArea::getLineNumBarWidth() const
{
    return impl_->getLineNumBarWidth();
}

void TextArea::drawEachLineNum(std::function<void(RowN lineNum, i32 baseline, const RowBound &bound, bool isLastAct)> &&action) const
{
    return impl_->drawEachLineNum(std::move(action));
}

void TextArea::drawEachChar(std::function<void(i32 x, i32 y, char32_t c)> &&action) const
{
    return impl_->drawEachChar(std::move(action));
}

Signal<void()> &TextArea::sigShouldRepaint()
{
    return impl_->sigShouldRepaint();
}

Signal<void()> &TextArea::sigViewLocChanged()
{
    return impl_->sigViewLocChanged();
}

}
