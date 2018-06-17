#pragma once

#include <functional>

#include <QFont>
#include <QFontMetrics>

#include "font_info.h"

class FontConfig
{
public:

    void AddFontChangeListener(std::function<void(const QFont &newFont)> &&listener)
    {
        m_fontChangeListeners.push_back(listener);
    }

    const QFont &GetFont() const
    {
        return m_font;
    }

    void SetFont(const QFont &font)
    {
        if (m_font != font)
        {
            m_font = font;

            for (auto listener : m_fontChangeListeners)
            {
                listener(m_font);
            }
        }
    }
    
    int GetCharMargin() const
    {
        return m_charMargin;
    }

    void SetCharMargin(int charMargin)
    {
        m_charMargin = charMargin;
    }

    int GetTabSize() const
    {
        return m_tabSize;
    }

    void SetTabSize(int tabSize)
    {
        m_tabSize = tabSize;
    }

private:
    QFont m_font;
    int m_charMargin = 1;
    int m_tabSize = 4;

    typedef std::function<void(const QFont &newFont)> FontChangeListener;
    typedef std::vector<FontChangeListener> FontChangeListeners;
    FontChangeListeners m_fontChangeListeners;
};

class FontInfo
{
public:
    FontInfo(FontConfig &drawConfig)
        : m_drawConfig(drawConfig)
        , m_metrics(drawConfig.GetFont())
    {
        drawConfig.AddFontChangeListener([this](const QFont &newFont) {
            Init();
        });

        Init();
    }

    void Init()
    {
        m_metrics = QFontMetrics(m_drawConfig.GetFont());
        m_isFixWide = m_metrics.width('i') == m_metrics.width('w');
        m_charWidthForFixWide = m_metrics.width('a');
    }

    bool IsFixWideChar() const
    {
        return m_isFixWide;
    }

    int GetCharWidth(wchar_t c) const;

    int GetFontAscent() const
    {
        return m_metrics.ascent();
    }

    int GetFontDescent() const
    {
        return m_metrics.descent();
    }

    int GetFontHeight() const
    {
        return m_metrics.height();
    }

    int GetLineHeight() const
    {
        return GetFontHeight() * 1.2;
    }

    const FontConfig &GetDrawConfig() const
    {
        return m_drawConfig;
    }

private:
    const FontConfig &m_drawConfig;
    QFontMetrics m_metrics;
    
private:
    bool m_isFixWide = false;
    int m_charWidthForFixWide = 0;
};

