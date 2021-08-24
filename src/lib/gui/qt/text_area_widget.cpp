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

static QImage::Format BuffImageFormat = QImage::Format_ARGB32_Premultiplied;

}


namespace gui::qt
{

static constexpr int DefaultWidth = 800;
static constexpr int DefaultHeight = 600;

TextAreaWidget::TextAreaWidget(TextArea &textArea)
    : textArea_(textArea)
    , textPaintBuff_(QSize(DefaultWidth, DefaultHeight), BuffImageFormat)
{
    resize(DefaultWidth, DefaultHeight);

    setCursor(Qt::IBeamCursor);
    setAttribute(Qt::WA_InputMethodEnabled);
    setFocusPolicy(Qt::ClickFocus);

    dirtyBuffFlags_.set(DirtyBuffFlag::Text);

    connect(this, &TextAreaWidget::qtSigShouldRepaint, [this] {
        dirtyBuffFlags_.set(DirtyBuffFlag::Text);
        update();
    });
    textArea_.sigShouldRepaint().connect([this] {
        emit qtSigShouldRepaint();
    });
}

TextAreaWidget::~TextAreaWidget()
{

}

QSize TextAreaWidget::sizeHint() const
{
    return QSize(textArea_.width(), textArea_.height());
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

    const QSize sz(size());

    if (e->oldSize().isValid() && sz != e->oldSize()) {
        textPaintBuff_ = std::move(QImage(sz, BuffImageFormat));
        dirtyBuffFlags_.set(DirtyBuffFlag::Text);
    }

    textArea_.resize({ sz.width(), sz.height() });

    update();
}

void TextAreaWidget::keyPressEvent(QKeyEvent *e)
{
    const int key = e->key();

    switch (key) {
    case Qt::Key_Up:
        textArea_.onDirUpKeyPress();
        update();
        break;
    case Qt::Key_Down:
        textArea_.onDirDownKeyPress();
        update();
        break;
    case Qt::Key_Left:
        textArea_.onDirLeftKeyPress();
        update();
        break;
    case Qt::Key_Right:
        textArea_.onDirRightKeyPress();
        update();
        break;
    case Qt::Key_S:
        textArea_.moveDownByOneLine();
        update();
        break;
    default:
        break;
    }
}

void TextAreaWidget::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        textArea_.onPrimaryButtomPress(e->x(), e->y());
        update();
    }
}

void TextAreaWidget::paintBackground(QPainter &p)
{
    p.fillRect(rect(), Qt::white);
}

void TextAreaWidget::paintLastActLine(QPainter &p)
{
    std::optional<Rect> r = textArea_.getLastActLineDrawRect();
    if (r) {
        p.fillRect(r->left(), r->top(), r->width(), r->height(),
            QColor(Qt::green).lighter(192));
    }
}

static inline QString unicodeToUtf16SurrogatePairs(char32_t unicode) {
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
    fontToQFont(textArea_.config().font(), qfont);
    p.setFont(qfont);

    textArea_.drawEachChar([&p](i32 x, i32 y, char32_t unicode) {
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
    std::optional<VerticalLine> vl = textArea_.getNormalCursorDrawData();
    if (vl) {
        p.drawLine(vl->x(), vl->top(), vl->x(), vl->bottom());
    }
}


}
