#include "text_area_widget.h"

#include <cassert>
#include <QPainter>
#include <QMouseEvent>

#include "gui/text_area.h"
#include "gui/text_area_config.h"
#include "editor/editor.h"
#include "font_to_qfont.h"


namespace
{

class AutoSaver {
public:
    explicit AutoSaver(QPainter &painter) : painter_(painter) {
        painter_.save();
    }
    ~AutoSaver() {
        painter_.restore();
    }
private:
    QPainter &painter_;
};

static QSize kSizeHint(800, 600);

static QImage::Format kBuffImageFormat = QImage::Format_ARGB32_Premultiplied;

}


namespace gui::qt
{


TextAreaWidget::TextAreaWidget(QWidget *parent)
    : QWidget(parent)
    , textPaintBuff_(kSizeHint, kBuffImageFormat)
{
    setCursor(Qt::IBeamCursor);
    setAttribute(Qt::WA_InputMethodEnabled);
    setFocusPolicy(Qt::ClickFocus);

    dirtyBuffFlags_.set(DirtyBuffFlag::Text);
}

TextAreaWidget::~TextAreaWidget()
{

}

QSize TextAreaWidget::sizeHint() const
{
    return kSizeHint;
}

void TextAreaWidget::paintEvent(QPaintEvent *e)
{
    if (dirtyBuffFlags_.has(DirtyBuffFlag::Text)) {
        prepareTextImage();
        dirtyBuffFlags_.unset(DirtyBuffFlag::Text);
    }

    QPainter p(this);
    paintWidget(p);
}

void TextAreaWidget::showEvent(QShowEvent *e)
{
    if (dirtyBuffFlags_.has(DirtyBuffFlag::Text)) {
        prepareTextImage();
        dirtyBuffFlags_.unset(DirtyBuffFlag::Text);
    }
}

void TextAreaWidget::resizeEvent(QResizeEvent *e)
{
    if (!area_) {
        return;
    }

    const QSize sz(size());

    if (e->oldSize().isValid() && sz != e->oldSize()) {
        textPaintBuff_ = std::move(QImage(sz, kBuffImageFormat));
        dirtyBuffFlags_.set(DirtyBuffFlag::Text);
    }

    area_->resize({ sz.width(), sz.height() });

    update();
}

void TextAreaWidget::keyPressEvent(QKeyEvent *e)
{
    if (!area_) {
        return;
    }

    const int key = e->key();

    switch (key) {
    case Qt::Key_Up:
        area_->onDirUpKeyPress();
        update();
        break;
    case Qt::Key_Down:
        area_->onDirDownKeyPress();
        update();
        break;
    case Qt::Key_Left:
        area_->onDirLeftKeyPress();
        update();
        break;
    case Qt::Key_Right:
        area_->onDirRightKeyPress();
        update();
        break;
    case Qt::Key_S:
        area_->moveDownByOneLine();
        update();
        break;
    default:
        break;
    }
}

void TextAreaWidget::mousePressEvent(QMouseEvent *e)
{
    if (!area_) {
        return;
    }

    if (e->button() == Qt::LeftButton) {
        area_->onPrimaryButtomPress(Pixel(e->x()), Pixel(e->y()));
        update();
    }
}

void TextAreaWidget::bind(TextArea *area)
{
    if (area_ == area) {
        return;
    }

    unbind();

    area_ = area;
    if (!area_) {
        return;
    }

    textAreaSigConns_ += area_->sigViewLocChanged().connect([this] {
        dirtyBuffFlags_.set(DirtyBuffFlag::Text);
        update();
    });

    area_->resize(Size(width(), height()));

    update();
}

void TextAreaWidget::unbind()
{
    if (area_) {
        area_ = nullptr;
    }

    textAreaSigConns_.clear();
}

void TextAreaWidget::paintBackground(QPainter &p)
{
    p.fillRect(rect(), Qt::white);
}

void TextAreaWidget::paintLastActLine(QPainter &p)
{
    if (!area_) {
        return;
    }

    std::optional<Rect> r = area_->getLastActLineDrawRect();
    if (r) {
        p.fillRect(r->left(), r->top(), r->width(), r->height(),
            QColor(Qt::green).lighter(192));
    }
}

static inline QString unicodeToUtf16SurrogatePairs(UChar unicode) {
    QChar high = QChar::highSurrogate(unicode);
    QChar low = QChar::lowSurrogate(unicode);
    QString surrogatedPairs;
    surrogatedPairs.push_back(high);
    surrogatedPairs.push_back(low);
    return surrogatedPairs;
}

void TextAreaWidget::prepareTextImage()
{
    if (!area_) {
        return;
    }

    QPainter p(&textPaintBuff_);
    textPaintBuff_.fill(QColor(0, 0, 0, 0));

    QFont qfont;
    fontToQFont(area_->config().font(), qfont);
    p.setFont(qfont);

    area_->drawEachChar([&p](Pixel::Raw x, Pixel::Raw y, UChar unicode) {
        if (!UCharUtil::needSurrogate(unicode)) {
            p.drawText(x, y, QChar(unicode));
        } else {
            p.drawText(x, y, unicodeToUtf16SurrogatePairs(unicode));
        }
    });
}

void TextAreaWidget::paintWidget(QPainter &p)
{
    paintBackground(p);
    paintLastActLine(p);
    p.drawImage(0, 0, textPaintBuff_);
    paintCursor(p);
}

void TextAreaWidget::paintCursor(QPainter &p)
{
    if (!area_) {
        return;
    }

    std::optional<VerticalLine> vl = area_->getNormalCursorDrawData();
    if (vl) {
        p.drawLine(vl->x(), vl->top(), vl->x(), vl->bottom());
    }
}


}
