#pragma once

#include <assert.h>
#include <vector>
#include <deque>
#include <bitset>

#include <QChar>

#include "doc/doc_define.h"
#include "doc/doc_loc.h"
#include "view/view_page.h"
#include "view/view_loc.h"
#include "view/view_rect.h"
#include "view/draw_vertical_line.h"
#include "view/view_define.h"

#include "util/callbacks.h"
#include "common/dir_enum.h"

namespace view
{
    class Point
    {
    public:
        Point() = default;
        Point(int x, int y) :m_x(x), m_y(y) {}
        int x() const { return m_x; }
        void setX(int x) { m_x = x; }
        int y() const { return m_y; }
        void setY(int y) { m_y = y; }
    private:
        int m_x = 0;
        int m_y = 0;
    };

    class Size
    {
    public:
        Size() {}
        Size(int width, int height) : m_width(width), m_height(height) {}
        int width() const { return m_width; }
        int height() const { return m_height; }
        void setWidth(int width) { m_width = width; }
        void setHeight(int height) { m_height = height; }
        bool operator==(const Size & b) const { return m_width == b.m_width && m_height == b.m_height; }
        bool operator!=(const Size & b) const { return !(*this == b); }
    private:
        int m_width = 0;
        int m_height = 0;
    };

    class Config;

    class LineBound
    {
    public:
        LineBound() = default;
        LineBound(int top, int bottom) :m_top(top), m_bottom(bottom) {}
        int top() const { return m_top; }
        void setTop(int top) { m_top = top; }
        int bottom() const { return m_bottom; }
        void setBottom(int bottom) { m_bottom = bottom; }
    private:
        int m_top = 0;
        int m_bottom = 0;
    };

    class RowBound
    {
    public:
        RowBound() = default;
        RowBound(int top, int height) :m_top(top), m_height(height) {}
        int top() const { return m_top; }
        void setTop(int top) { m_top = top; }
        int height() const { return m_height; }
        void setHeight(int height) { m_height = height; }
    private:
        int m_top = 0;
        int m_height = 0;
    };

} // namespace view

class ViewLoc
{
public:
    ViewLoc() { m_flag.set(flgNull); }
    explicit ViewLoc(RowN row) : m_row(row), m_line(0) {}
    ViewLoc(RowN row, LineN line) : m_row(row), m_line(line) {}

    bool isNull() const { return m_flag.test(flgNull); }
    bool atEnd() const { return m_flag.test(flgAtEnd); }
    
    RowN row() const { return m_row; }
    void setRow(RowN row) { m_row = row; }

    LineN line() const { return m_line; }
    void setLine(LineN line) { m_line = line; }

    bool operator==(const ViewLoc & b) const
    {
        return m_row == b.m_row && m_line == b.m_line && m_flag == b.m_flag;
    }
    bool operator!=(const ViewLoc & b) const
    {
        return !(*this == b);
    }

private:
    enum Flag
    {
        flgNull,
        flgAtEnd,

        flgCnt,
    };

private:
    RowN m_row = 0;
    LineN m_line = 0;
    std::bitset<flgCnt> m_flag;
};

class Row;
class DocLoc;
class Editor;

/*
��ͼ
֧�ֿ��ĵ�ĳһ��
�磬����������߼�������ӿ�֧��
*/
class View
{
public:
    View(Editor * editor, view::Config *config);

    ~View();

public:

    void initSize(const view::Size &size);

    void onResize(const view::Size & size);

    void onPrimaryButtomPress(int x, int y);

    void onDirKeyPress(Dir dir);

	// ����ƶ�һ��line���ƶ��ɹ��򷵻�true���ƶ�ʧ���򷵻�false
	// ������ͼ��ֻ��ʾ�ĵ����һ��line���ĵ�û������ʱ������false
	bool moveDownByOneLine();

public:

    const view::Config &config() const { return m_config; }

    int getBaseLineByLineOffset(int off) const;

    view::Rect getLastActLineDrawRect() const;

    draw::VerticalLine getNormalCursorDrawData() const;

    int getLineNumBarWidth() const;

    void drawEachLineNum(std::function<void(RowN lineNum, int baseline, const view::RowBound & bound, bool isLastAct)> && action) const;

    void drawEachChar(std::function<void(int x, int y, QChar c)> && action) const;


public:
    CallbackHandle addOnUpdateListener(std::function<void()> && action);
    CallbackHandle addOnViewLocChangeListener(std::function<void()> && action);

private:
    int getMaxShownLineCnt() const;
    int getLineOffsetByRowIndex(int row) const;
    int getLineOffsetByLineLoc(const view::LineLoc & loc) const;
    int getLineOffsetByY(int y) const;
    view::LineLoc getLineLocByLineOffset(int offset) const;
    view::CharLoc getCharLocByLineLocAndX(const view::LineLoc & lineLoc, int x) const;
    view::CharLoc getCharLocByPoint(int x, int y) const;
    DocLoc getDocLocByPoint(int x, int y) const;
    view::RowLoc convertToRowLoc(RowN row) const;
    view::CharLoc convertToCharLoc(const DocLoc & docLoc) const;
    DocLoc convertToDocLoc(const view::CharLoc & charLoc) const;
    const view::Char & getChar(const view::CharLoc & charLoc) const;
    int getXByCharLoc(const view::CharLoc & charLoc) const;
    view::LineBound getLineBoundByLineOffset(int lineOffset) const;
    view::LineBound getLineBound(const view::LineLoc & lineLoc) const;
    view::RowBound getRowBound(const view::RowLoc & rowLoc) const;
    bool hasPrevCharAtSameLine(const view::CharLoc & charLoc) const;
    bool noPrevCharAtSameLine(const view::CharLoc & charLoc) const;
    bool hasNextCharAtSameLine(const view::CharLoc & charLoc) const;
    bool noNextCharAtSameLine(const view::CharLoc & charLoc) const;
	bool needEnsureHasNextLine(const view::CharLoc & charLoc) const;
    bool isLastLineOfRow(const view::LineLoc & lineLoc) const;
    bool isEndOfVirtualLine(const view::CharLoc & charLoc) const;
    view::CharLoc betterLocForVerticalMove(const view::CharLoc & charLoc) const;
    DocLoc getNextUpLoc(const DocLoc & docLoc) const;
    DocLoc getNextDownLoc(const DocLoc & docLoc) const;
    void makeVRow(const Row & row, view::VRow & vrow) const;
    void makeVRowWithWrapLine(const Row & row, view::VRow & vrow) const;
    void makeVRowNoWrapLine(const Row & row, view::VRow & vrow) const;

	// ��ͼ�����һ������line��LineLoc
	view::LineLoc getShownLastLineLoc() const;

private:
	// ���������������Ҫ�ں��ʵ�ʱ�̵���removeSpareRow
    void ensureHasPrevLine(const view::LineLoc & curLineLoc);

	// ȷ������������һ��
	// �����Ҫ��ҳ��ͷ������ƶ��򷵻�true
	// ����true����Ҫ�ں��ʵ�ʱ�̵���movePageHeadOneLine
    bool ensureHasNextLine(const view::LineLoc & curLineLoc);
    void onDirUpKeyPress();
    void onDirDownKeyPress();
    void onDirLeftKeyPress();
    void onDirRightKeyPress();
    void setViewLoc(const ViewLoc & viewLoc);

private:
    void remakePage();

	// ��ҳ�濪ͷ����ƶ�һ��line
	void movePageHeadOneLine();
	// ɾ�������row
    void removeSpareRow();

private:
    Editor & m_editor;
    const view::Config & m_config;
    view::Page m_page;
    view::Size m_size;
    ViewLoc m_loc{ 10, 0 };
    
    // ���ڷǵȿ����壬������������ƶ�ʱ��ϣ������������ȶ�����¼һ���ȶ�λ�ã�ÿ�������ƶ�ʱ������ѡȡ��֮�ӽ���λ��
    // ��ĳЩ��������£��������ƶ����Ȳ���
    int m_stable_x = 0;

private:
    Callbacks<void()> m_onUpdateListeners;
    Callbacks<void()> m_onViewLocChangeListeners;

private:
    CallbackHandle m_listenerIdForLastActLineUpdate;
};
