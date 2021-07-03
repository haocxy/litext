#pragma once

#include <vector>
#include "cursor.h"
#include "util/callbacks.h"

class Doc;

/*
逻辑上操作文档
用于提供操作文档数据的接口
如，选择文档中某些字符由这个接口支持
*/
class Editor
{
public:
    explicit Editor(Doc * model);

    const Doc & doc() const { return m_model; }

    const DocCursor &normalCursor() { return m_normalCursor; }

    RowN lastActRow() { return m_lastActRow; }

    void onPrimaryKeyPress(const DocLoc & loc);

    void setNormalCursor(const DocLoc & loc);

    void setNormalCursor(const DocCursor & cursor);

    // 以字符为单位，获得向左移动光标时的下一个字符位置
    DocLoc getNextLeftLocByChar(const DocLoc & loc) const;

    // 以字符为单位，获得向右移动光标时的下一个字符位置
    DocLoc getNextRightLocByChar(const DocLoc & loc) const;

public:
    CallbackHandle addOnLastActRowUpdateListener(std::function<void()> && action);

private:
    void setLastActRow(RowN row);

private:

    Doc & m_model;

    // 普通模式光标
    DocCursor m_normalCursor;

    // 最后一次活跃的行
    RowN m_lastActRow = -1;

private:
    Callbacks<void()> m_lastActRowUpdateListeners;
};
