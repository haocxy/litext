#include "textpad2.h"

#include <assert.h>
#include <QPainter>
#include <QMouseEvent>

#include "module/view/view.h"
#include "module/view/view_config.h"
#include "module/control/doc_controller.h"

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

TextPad2::TextPad2(View *view, DocController *controller, QWidget *parent)
    : m_view(*view)
    , m_controller(*controller)
    , QWidget(parent)
{
    assert(view);
    assert(controller);

    setCursor(Qt::IBeamCursor);
    setAttribute(Qt::WA_InputMethodEnabled);
    setFocus();
}

TextPad2::~TextPad2()
{

}

void TextPad2::paintEvent(QPaintEvent * e)
{
    QPainter p(this);
    paintBackground(p);
    paintTextContent(p);
    paintCursor(p);
}

void TextPad2::showEvent(QShowEvent * e)
{
    QSize sz(size());
    m_view.Init(0, { sz.width(), sz.height() });
}

void TextPad2::resizeEvent(QResizeEvent * e)
{
    QSize sz(size());
    m_view.Init(0, { sz.width(), sz.height() });
}

void TextPad2::keyPressEvent(QKeyEvent * e)
{

}

void TextPad2::mousePressEvent(QMouseEvent * e)
{
    if (e->button() == Qt::LeftButton)
    {
        const DocAddr da = m_view.getDocAddrByPoint(e->x(), e->y());
        m_controller.onPrimaryKeyPress(da);
        update();
    }
}

void TextPad2::paintBackground(QPainter & p)
{
    AutoSaver as(p);

    p.fillRect(rect(), QColor(Qt::white));
}

void TextPad2::paintTextContent(QPainter & p)
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
        for (const v::Line &line : phase)
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

void TextPad2::paintCursor(QPainter & p)
{
    enum { kBottomShrink = 2 };

    const DocCursor & cursor = m_controller.normalCursor();

    if (cursor.isNull())
    {
        return;
    }

    if (cursor.isInsert())
    {
        const DocAddr & docAddr = cursor.addr();
        const view::CharAddr &vcAddr = m_view.convertToCharAddr(docAddr);
        if (!vcAddr.isNull())
        {
            const view::Char &vc = m_view.getChar(vcAddr);
            const view::LineBound bound = m_view.getLineBound(vcAddr);
            const int x = vc.x() - 1;
            p.drawLine(x, bound.top() + kBottomShrink,
                       x, bound.bottom() - kBottomShrink);
        }
    }
}


