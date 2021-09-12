#include "text_area.h"

#include "text_area_impl.h"


namespace gui
{

TextArea::TextArea(Editor &editor, const TextAreaConfig &config)
    : impl_(new TextAreaImpl(editor, config))
{
}

TextArea::~TextArea()
{
    WriteLock lock(mtx_);
    delete impl_;
    impl_ = nullptr;
}

const doc::Document &TextArea::doc() const
{
    ReadLock lock(mtx_);
    return impl_->doc();
}

doc::Document &TextArea::doc()
{
    ReadLock lock(mtx_);
    return impl_->doc();
}

void TextArea::open(const Size &size, RowN row)
{
    ReadLock lock(mtx_);
    impl_->open(size, row);
}

void TextArea::resize(const Size &size)
{
    ReadLock lock(mtx_);
    impl_->resize(size);
}

void TextArea::jumpTo(RowN row)
{
    ReadLock lock(mtx_);
    impl_->jumpTo(row);
}

void TextArea::jumpTo(float ratio)
{
    ReadLock lock(mtx_);
    impl_->jumpTo(ratio);
}

RowN TextArea::rowOff() const
{
    ReadLock lock(mtx_);
    return impl_->rowOff();
}

ScrollRatio TextArea::scrollRatio() const
{
    ReadLock lock(mtx_);
    return impl_->scrollRatio();
}

LineN TextArea::lineCountLimit() const
{
    ReadLock lock(mtx_);
    return impl_->lineCountLimit();
}

void TextArea::moveCursor(Dir dir)
{
    ReadLock lock(mtx_);
    impl_->moveCursor(dir);
}

void TextArea::putCursor(i32 x, i32 y)
{
    ReadLock lock(mtx_);
    impl_->putCursor(x, y);
}

bool TextArea::moveDownByOneLine()
{
    ReadLock lock(mtx_);
    return impl_->moveDownByOneLine();
}

const QImage &TextArea::widgetImg() const
{
    ReadLock lock(mtx_);
    return impl_->widgetImg();
}

int TextArea::width() const
{
    ReadLock lock(mtx_);
    return impl_->width();
}

int TextArea::height() const
{
    ReadLock lock(mtx_);
    return impl_->height();
}

Editor &TextArea::editor()
{
    ReadLock lock(mtx_);
    return impl_->editor();
}

const TextAreaConfig &TextArea::config() const
{
    ReadLock lock(mtx_);
    return impl_->config();
}

void TextArea::drawEachLineNum(std::function<void(RowN lineNum, i32 baseline, const RowBound &bound, bool isLastAct)> &&action) const
{
    ReadLock lock(mtx_);
    return impl_->drawEachLineNum(std::move(action));
}

Signal<void()> &TextArea::sigShouldRepaint()
{
    ReadLock lock(mtx_);
    return impl_->sigShouldRepaint();
}

Signal<void()> &TextArea::sigViewportChanged()
{
    ReadLock lock(mtx_);
    return impl_->sigViewportChanged();
}

}
