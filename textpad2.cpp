#include "textpad2.h"

#include <assert.h>
#include <QPainter>

#include "module/view/view.h"
#include "module/view/view_config.h"

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

TextPad2::TextPad2(View *view, QWidget *parent)
    : m_view(*view)
    , QWidget(parent)
{
    assert(view != nullptr);

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


