#pragma once

#include <vector>

#include "core/callbacks.h"
#include "core/ustring.h"
#include "core/worker.h"
#include "doc/declare_async_doc_server.h"
#include "doc/row_range.h"
#include "cursor.h"


class Doc;

/*
逻辑上操作文档
用于提供操作文档数据的接口
如，选择文档中某些字符由这个接口支持
*/
class Editor
{
public:

    Editor(Doc * model, doc::AsyncDocServer &docServer);

    const Doc & doc() const { return m_model; }

    doc::AsyncDocServer &server() { return docServer_; }

    const DocCursor &normalCursor() { return m_normalCursor; }

    RowN lastActRow() { return m_lastActRow; }

    void onPrimaryKeyPress(const DocLoc & loc);

    void setNormalCursor(const DocLoc & loc);

    void setNormalCursor(const DocCursor & cursor);

    // 以字符为单位，获得向左移动光标时的下一个字符位置
    DocLoc getNextLeftLocByChar(const DocLoc & loc) const;

    // 以字符为单位，获得向右移动光标时的下一个字符位置
    DocLoc getNextRightLocByChar(const DocLoc & loc) const;

    void loadRows(const doc::RowRange &range, std::function<void(std::vector<UString> &&rows)> &&cb);

    void queryRowCount(std::function<void(RowN)> &&cb);

public:
    CallbackHandle addOnLastActRowUpdateListener(std::function<void()> && action);

private:
    void setLastActRow(RowN row);

private:
    Doc & m_model;

    doc::AsyncDocServer &docServer_;

    // 普通模式光标
    DocCursor m_normalCursor;

    // 最后一次活跃的行
    RowN m_lastActRow = -1;

private:
    Callbacks<void()> m_lastActRowUpdateListeners;
};
