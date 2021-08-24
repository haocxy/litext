#include "text_area.h"

#include "text_area_impl.h"


namespace gui
{

TextArea::TextArea(Editor &editor, const TextAreaConfig &config, const Size &size)
    : impl_(new impl::TextArea(editor, config, size))
{
}

TextArea::~TextArea()
{
    delete impl_;
    impl_ = nullptr;
}

void TextArea::start()
{
    impl_->start();
}

void TextArea::resize(const Size &size)
{
    impl_->resize(size);
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
