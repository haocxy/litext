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
        AutoSaver(QPainter & painter) : m_painter(painter) {
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
}

TextPad::TextPad(View *view, QWidget *parent)
    : m_view(*view)
    , QWidget(parent)
{
    assert(view);

    setCursor(Qt::IBeamCursor);
    setAttribute(Qt::WA_InputMethodEnabled);
    setFocusPolicy(Qt::ClickFocus);
}

TextPad::~TextPad()
{

}

void TextPad::paintEvent(QPaintEvent * e)
{
    QPainter p(this);
    paintBackground(p);
    paintLastActLine(p);
    paintTextContent(p);
    paintCursor(p);
}

void TextPad::showEvent(QShowEvent * e)
{
    QSize sz(size());
    m_view.onShow(0, { sz.width(), sz.height() });
}

void TextPad::resizeEvent(QResizeEvent * e)
{
    QSize sz(size());
    m_view.onShow(0, { sz.width(), sz.height() });
}

void TextPad::keyPressEvent(QKeyEvent * e)
{
    const int key = e->key();

    switch (key)
    {
    case Qt::Key_Down:
        m_view.onDirectionKeyPress(Dir::Down);
        update();
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
        update();
    }
}

void TextPad::paintBackground(QPainter & p)
{
    AutoSaver as(p);

    p.fillRect(rect(), QColor(Qt::white));
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

    AutoSaver as(p);

    QFont qfont;
    fillQFont(m_view.config().font(), qfont);
    p.setFont(qfont);

    const v::Page &page = m_view.page();

    int lineOffset = 0;

    for (const v::Phase &phase : page)
    {
        for (const v::DocLine &line : phase)
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

void TextPad::paintCursor(QPainter & p)
{
    const draw::VerticalLine vl = m_view.getNormalCursorDrawData();

    if (!vl.isNull())
    {
        p.drawLine(vl.x(), vl.top(), vl.x(), vl.bottom());
    }
}


