#include "text_area_widget.h"

#include <cassert>
#include <QPainter>
#include <QMouseEvent>

#include "gui/text_area.h"
#include "gui/text_area_config.h"
#include "editor/editor.h"
#include "qt_util.h"


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


TextAreaWidget::TextAreaWidget(gui::TextArea *view, QWidget *parent)
    : QWidget(parent)
    , view_(*view)
    , textPaintBuff_(kSizeHint, kBuffImageFormat)
{
    assert(view);

    setCursor(Qt::IBeamCursor);
    setAttribute(Qt::WA_InputMethodEnabled);
    setFocusPolicy(Qt::ClickFocus);

    view_.initSize(gui::Size(textPaintBuff_.width(), textPaintBuff_.height()));

    prepareTextImage();

    cbhViewLocChanged_ = view_.addOnViewLocChangeListener([this] {
        dirtyBuffFlags_.set(DirtyBuffFlag::Text);
        });
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

}

void TextAreaWidget::resizeEvent(QResizeEvent *e)
{
    const QSize sz(size());

    if (e->oldSize().isValid() && sz != e->oldSize()) {
        textPaintBuff_ = std::move(QImage(sz, kBuffImageFormat));
        dirtyBuffFlags_.set(DirtyBuffFlag::Text);
    }

    view_.resize({ sz.width(), sz.height() });

    refresh();
}

void TextAreaWidget::keyPressEvent(QKeyEvent *e)
{
    const int key = e->key();

    switch (key) {
    case Qt::Key_Up:
        view_.onDirUpKeyPress();
        refresh();
        break;
    case Qt::Key_Down:
        view_.onDirDownKeyPress();
        refresh();
        break;
    case Qt::Key_Left:
        view_.onDirLeftKeyPress();
        refresh();
        break;
    case Qt::Key_Right:
        view_.onDirRightKeyPress();
        refresh();
        break;
    case Qt::Key_S:
        view_.moveDownByOneLine();
        refresh();
        break;
    default:
        break;
    }
}

void TextAreaWidget::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        view_.onPrimaryButtomPress(e->x(), e->y());
        refresh();
    }
}

void TextAreaWidget::paintBackground(QPainter &p)
{
    p.fillRect(rect(), Qt::white);
}

void TextAreaWidget::paintLastActLine(QPainter &p)
{
    std::optional<gui::Rect> r = view_.getLastActLineDrawRect();
    if (r) {
        p.fillRect(r->left(), r->top(), r->width(), r->height(), QColor(Qt::green).lighter(192));
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
    QPainter p(&textPaintBuff_);
    textPaintBuff_.fill(QColor(0, 0, 0, 0));

    QFont qfont;
    QtUtil::fillQFont(view_.config().font(), qfont);
    p.setFont(qfont);

    view_.drawEachChar([&p](int x, int y, UChar unicode) {
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

void TextAreaWidget::refresh()
{
    // refresh这个函数里即使只调用了update，也要这样包装一层
    // 其他部分调用refresh，这样就便于以后需要时添加重绘相关的公用逻辑
    update();
}

void TextAreaWidget::paintCursor(QPainter &p)
{
    std::optional<gui::VerticalLine> vl = view_.getNormalCursorDrawData();
    if (vl) {
        p.drawLine(vl->x(), vl->top(), vl->x(), vl->bottom());
    }
}


}
