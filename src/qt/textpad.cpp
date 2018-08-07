#include "textpad.h"

#include <assert.h>
#include <QPainter>
#include <QMouseEvent>

#include "view/view.h"
#include "view/view_config.h"
#include "editor/editor.h"

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

    void fillQFont(const view::Font &font, QFont &qfont)
    {
        qfont.setFamily(font.family().c_str());
        qfont.setPointSize(font.size());
        qfont.setBold(font.bold());
    }

    static int kWidthHint = 800;
    static int kHeightHint = 600;
    static QSize kSizeHint(kWidthHint, kHeightHint);
}

TextPad::TextPad(View *view, QWidget *parent)
    : QWidget(parent)
    , m_view(*view)
    , m_buff(kWidthHint, kHeightHint)
{
    assert(view);

    setCursor(Qt::IBeamCursor);
    setAttribute(Qt::WA_InputMethodEnabled);
    setFocusPolicy(Qt::ClickFocus);

    m_view.initSize({kWidthHint,kHeightHint});

    paintOnPixmap();
}

TextPad::~TextPad()
{

}

QSize TextPad::sizeHint() const
{
    return kSizeHint;
}

void TextPad::paintEvent(QPaintEvent * e)
{
    QPainter p(this);
    p.drawPixmap(rect(), m_buff);
}

void TextPad::showEvent(QShowEvent * e)
{

}

void TextPad::resizeEvent(QResizeEvent * e)
{
    QSize sz(size());

    if (e->oldSize().isValid() && sz != e->oldSize())
    {
        m_buff = std::move(QPixmap(sz));
    }

    m_view.onResize({ sz.width(), sz.height() });

    refresh();
}

void TextPad::keyPressEvent(QKeyEvent * e)
{
    const int key = e->key();

    switch (key)
    {
    case Qt::Key_Up:
        m_view.onDirKeyPress(Dir::Up);
        refresh();
        break;
    case Qt::Key_Down:
        m_view.onDirKeyPress(Dir::Down);
        refresh();
        break;
    case Qt::Key_Left:
        m_view.onDirKeyPress(Dir::Left);
        refresh();
        break;
    case Qt::Key_Right:
        m_view.onDirKeyPress(Dir::Right);
        refresh();
        break;
    default:
        break;
    }
}

void TextPad::mousePressEvent(QMouseEvent * e)
{
    if (e->button() == Qt::LeftButton)
    {
        m_view.onPrimaryButtomPress(e->x(), e->y());
        refresh();
    }
}

void TextPad::paintBackground(QPainter & p)
{
    p.fillRect(m_buff.rect(), QColor(Qt::white));
}

void TextPad::paintLastActLine(QPainter & p)
{
    view::Rect r = m_view.getLastActLineDrawRect();
    if (r.isNull())
    {
        return;
    }

    p.fillRect(r.left(), r.top(), r.width(), r.height(), QColor(Qt::green).lighter(192));
}

void TextPad::paintTextContent(QPainter & p)
{
    namespace v = view;

    QFont qfont;
    fillQFont(m_view.config().font(), qfont);
    p.setFont(qfont);

    const v::Page &page = m_view.page();

    int lineOffset = 0;

    for (const v::VRow & row : page)
    {
        for (const v::Line &line : row)
        {
            const int baseline = m_view.getBaseLineByLineOffset(lineOffset);

            for (const v::Char &c : line)
            {
                p.drawText(c.x(), baseline, QChar(c.unicode()));
            }

            ++lineOffset;
        }
    }
}

void TextPad::paintOnPixmap()
{
    QPainter p(&m_buff);
    paintBackground(p);
    paintLastActLine(p);
    paintTextContent(p);
    paintCursor(p);
}

void TextPad::refresh()
{
    paintOnPixmap();
    update();
}

void TextPad::paintCursor(QPainter & p)
{
    const draw::VerticalLine vl = m_view.getNormalCursorDrawData();

    if (!vl.isNull())
    {
        p.drawLine(vl.x(), vl.top(), vl.x(), vl.bottom());
    }
}


