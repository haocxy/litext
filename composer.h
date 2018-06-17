#pragma once

#include <vector>
#include <deque>

#include "doc_model_define.h"

class DocModel;
class FontInfoProvider;

class CharComp
{
public:
    
    ColIndex GetColIndex() const
    {
        return m_colIndex;
    }

    void SetColIndex(ColIndex colIndex)
    {
        m_colIndex = colIndex;
    }

    wchar_t GetChar() const { return m_ch; }
    void SetChar(wchar_t ch) { m_ch = ch; }

    int GetLeftX() const { return m_leftX; }
    void SetLeftX(int leftX) { m_leftX = leftX; }

    int GetCharWidth() const { return m_charWidth; }
    void SetCharWidth(int charWidth) { m_charWidth = charWidth; }

private:
    ColIndex m_colIndex = 0;
    wchar_t m_ch = 0;
    int m_leftX = 0;
    int m_charWidth = 0;
};

class SubLineComp
{
public:
    typedef std::vector<CharComp> CharComps;
public:
    CharComp &Grow()
    {
        const auto size = m_charComps.size();
        m_charComps.resize(size + 1);
        CharComp &charComp = m_charComps[size];
        charComp.SetColIndex(static_cast<ColIndex>(size));
        return charComp;
    }

    int GetSubLineIndex() const
    {
        return m_subLineIndex;
    }

    void SetSubLineIndex(int subLineIndex)
    {
        m_subLineIndex = subLineIndex;
    }

    int GetLineIndex() const
    {
        return m_lineIndex;
    }

    void SetLineIndex(int lineIndex)
    {
        m_lineIndex = lineIndex;
    }

    RowIndex GetLineModelIndex() const
    {
        return m_lineModelIndex;
    }

    void SetLineModelIndex(RowIndex modelIndex)
    {
        m_lineModelIndex = modelIndex;
    }

    ColCnt GetColOffset() const
    {
        return m_colOffset;
    }

    void SetColOffset(ColCnt colOffset)
    {
        m_colOffset = colOffset;
    }

    ColCnt GetColCnt() const
    {
        return static_cast<ColCnt>(m_charComps.size());
    }

    CharComps::const_iterator begin() const
    {
        return m_charComps.begin();
    }

    CharComps::const_iterator end() const
    {
        return m_charComps.end();
    }

    bool IsLineEnd() const
    {
        return m_lineEnd;
    }

    void SetLineEnd()
    {
        m_lineEnd = true;
    }

private:
    int m_subLineIndex = 0;
    int m_lineIndex = 0;
    bool m_lineEnd = false;
    RowIndex m_lineModelIndex = 0;
    ColCnt m_colOffset = 0;
    CharComps m_charComps;
};

class LineComp
{
public:
    typedef std::vector<SubLineComp> SubLineComps;
public:

    SubLineComp * Grow(int lineIndex)
    {
        const auto size = m_subLines.size();
        m_subLines.resize(size + 1);
        SubLineComp &subLineComp = m_subLines[size];
        subLineComp.SetSubLineIndex(static_cast<int>(size));
        subLineComp.SetLineIndex(lineIndex);
        subLineComp.SetLineModelIndex(m_modelIndex);
        subLineComp.SetColOffset(GetColOffset());
        return &subLineComp;
    }

    RowIndex GetModelIndex() const
    {
        return m_modelIndex;
    }

    void SetModelIndex(RowIndex modelIndex)
    {
        m_modelIndex = modelIndex;
    }

    int GetLineIndex() const
    {
        return m_lineIndex;
    }

    void SetLineIndex(int lineIndex)
    {
        m_lineIndex = lineIndex;
    }

    int GetSubLineCnt() const
    {
        return static_cast<int>(m_subLines.size());
    }

    SubLineComps::const_iterator begin() const
    {
        return m_subLines.begin();
    }

    SubLineComps::const_iterator end() const
    {
        return m_subLines.end();
    }

    const SubLineComps &GetSubLines() const
    {
        return m_subLines;
    }

private:
    ColCnt GetColOffset() const
    {
        ColCnt col = 0;
        for (const SubLineComp &sub : m_subLines)
        {
            col += sub.GetColCnt();
        }
        return col;
    }

private:
    RowIndex m_modelIndex = 0;
    int m_lineIndex = 0;
    SubLineComps m_subLines;
};

class Composer
{
public:
    typedef std::deque<LineComp> LineComps;
public:
    Composer(const DocModel *model, const FontInfoProvider *fip)
        : m_model(*model)
        , m_fip(*fip)
    {

    }

    void Init(int areaWidth, int areaHeight, RowIndex modelRowIndex)
    {
        m_areaWidth = areaWidth;
        m_areaHeight = areaHeight;
        m_modelRowIndex = modelRowIndex;

        DoInit();
    }

    void UpdateAreaSize(int width, int height)
    {
        const int oldWidth = m_areaWidth;
        const int oldHeight = m_areaHeight;

        if (m_areaWidth != width && m_areaWidth != height)
        {
            m_areaWidth = width;
            m_areaHeight = height;
            BothWithAndHeightChange(oldWidth, oldHeight, width, height);
            return;
        }

        if (m_areaWidth == width && m_areaHeight == height)
        {
            return;
        }

        if (m_areaWidth != width)
        {
            m_areaWidth = width;
            OnlyWidthChange(oldWidth, width);
        }
        else
        {
            m_areaHeight = height;
            OnlyHeightChange(oldHeight, height);
        }
    }

    void UpdateModelRowIndex(RowIndex rowIndex)
    {
        if (m_modelRowIndex != rowIndex)
        {
            const RowIndex oldIndex = m_modelRowIndex;
            m_modelRowIndex = rowIndex;
            AfterModelRowIndexChange(oldIndex, rowIndex);
        }
    }

    const LineComps &GetLineComps() const
    {
        return m_lines;
    }

    LineComps::const_iterator begin() const
    {
        return m_lines.begin();
    }

    LineComps::const_iterator end() const
    {
        return m_lines.end();
    }

private:
    void DoInit();

    void OnlyWidthChange(int oldWidth, int newWidth);

    void OnlyHeightChange(int oldHeight, int newHeight);

    void BothWithAndHeightChange(int oldWidth, int oldHeight, int newWidth, int newHeight);

    void AfterModelRowIndexChange(RowIndex oldIndex, RowIndex newIndex);

private:
    LineComp & Grow(RowIndex modelRowIndex)
    {
        const auto size = m_lines.size();
        m_lines.resize(size + 1);
        LineComp &res = m_lines[size];
        res.SetModelIndex(modelRowIndex);
        res.SetLineIndex(static_cast<int>(size));
        return res;
    }

private:
    
    const DocModel &m_model;
    const FontInfoProvider &m_fip;
    int m_areaWidth = 0;
    int m_areaHeight = 0;
    int m_modelRowIndex = 0;
    std::deque<LineComp> m_lines;
};
