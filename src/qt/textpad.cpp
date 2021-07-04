#include "textpad.h"

#include <assert.h>
#include <QPainter>
#include <QMouseEvent>
#include <QDebug>

#include "view/view.h"
#include "config/view_config.h"
#include "editor/editor.h"
#include "qt_util.h"

namespace
{
    class AutoSaver
    {
    public:
        explicit AutoSaver(QPainter & painter) : m_painter(painter) {
            m_painter.save();
        }
        ~AutoSaver() {
            m_painter.restore();
        }
    private:
        QPainter & m_painter;
    };



    static int kWidthHint = 800;
    static int kHeightHint = 600;
    static QSize kSizeHint(kWidthHint, kHeightHint);
    static QImage::Format kBuffImageFormat = QImage::Format_ARGB32_Premultiplied;
}

TextAreaWidget::TextAreaWidget(View *view, QWidget *parent)
    : QWidget(parent)
    , m_view(*view)
    , m_textBuff(kWidthHint, kHeightHint, kBuffImageFormat)
{
    assert(view);

    setCursor(Qt::IBeamCursor);
    setAttribute(Qt::WA_InputMethodEnabled);
    setFocusPolicy(Qt::ClickFocus);

    m_view.initSize({kWidthHint,kHeightHint});

    prepareTextImage();

    m_listenerHandleViewLocChange = m_view.addOnViewLocChangeListener([this] { m_dirtyBuffFlags.set(DBF_Text); });
}

TextAreaWidget::~TextAreaWidget()
{

}

QSize TextAreaWidget::sizeHint() const
{
    return kSizeHint;
}

void TextAreaWidget::paintEvent(QPaintEvent * e)
{
    if (m_dirtyBuffFlags.test(DBF_Text))
    {
        prepareTextImage();
        m_dirtyBuffFlags.set(DBF_Text, false);
    }

    QPainter p(this);
    paintWidget(p);
}

void TextAreaWidget::showEvent(QShowEvent * e)
{

}

void TextAreaWidget::resizeEvent(QResizeEvent * e)
{
    QSize sz(size());

    if (e->oldSize().isValid() && sz != e->oldSize())
    {
        m_textBuff = std::move(QImage(sz, kBuffImageFormat));
        m_dirtyBuffFlags.set(DBF_Text);
    }

    m_view.onResize({ sz.width(), sz.height() });

    refresh();
}

void TextAreaWidget::keyPressEvent(QKeyEvent * e)
{
    const int key = e->key();

    switch (key)
    {
    case Qt::Key_Up:
        m_view.onDirUpKeyPress();
        refresh();
        break;
    case Qt::Key_Down:
        m_view.onDirDownKeyPress();
        refresh();
        break;
    case Qt::Key_Left:
        m_view.onDirLeftKeyPress();
        refresh();
        break;
    case Qt::Key_Right:
        m_view.onDirRightKeyPress();
        refresh();
        break;
	case Qt::Key_S:
		m_view.moveDownByOneLine();
		refresh();
		break;
    default:
        break;
    }
}

void TextAreaWidget::mousePressEvent(QMouseEvent * e)
{
    if (e->button() == Qt::LeftButton)
    {
        m_view.onPrimaryButtomPress(e->x(), e->y());
        refresh();
    }
}

void TextAreaWidget::paintBackground(QPainter & p)
{
    p.fillRect(rect(), Qt::white);
}

void TextAreaWidget::paintLastActLine(QPainter & p)
{
    std::optional<view::Rect> r = m_view.getLastActLineDrawRect();
    if (!r)
    {
        return;
    }

    p.fillRect(r->left(), r->top(), r->width(), r->height(), QColor(Qt::green).lighter(192));
}

void TextAreaWidget::prepareTextImage()
{
    QPainter p(&m_textBuff);
    m_textBuff.fill(QColor(0, 0, 0, 0));

    QFont qfont;
    QtUtil::fillQFont(m_view.config().font(), qfont);
    p.setFont(qfont);

    m_view.drawEachChar([&p](int x, int y, UChar c) {
        if (!UCharUtil::needSurrogate(c)) {
            p.drawText(x, y, QChar(c));
        } else {
            QChar high = QChar::highSurrogate(c);
            QChar low = QChar::lowSurrogate(c);
            QString surrogatedChar;
            surrogatedChar.push_back(high);
            surrogatedChar.push_back(low);
            p.drawText(x, y, surrogatedChar);
        }
    });
}

void TextAreaWidget::paintWidget(QPainter & p)
{
    paintBackground(p);
    paintLastActLine(p);
    p.drawImage(0, 0, m_textBuff);
    paintCursor(p);
}

void TextAreaWidget::refresh()
{
    // refresh这个函数里即使只调用了update，也要这样包装一层
    // 其他部分调用refresh，这样就便于以后需要时添加重绘相关的公用逻辑
    update();
}

void TextAreaWidget::paintCursor(QPainter & p)
{
    std::optional<draw::VerticalLine> vl = m_view.getNormalCursorDrawData();

    if (vl)
    {
        p.drawLine(vl->x(), vl->top(), vl->x(), vl->bottom());
    }
}


