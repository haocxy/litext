#pragma once

#include <vector>

#include "core/fs.h"
#include "core/signal.h"
#include "doc/document.h"

#include "cursor.h"


class Editor
{
public:

    Editor(const fs::path &file);

    void start();

    const doc::Document &doc() const { return document_; }

    doc::Document &doc() { return document_; }

    const doc::Document &document() const { return document_; }

    doc::Document &document() { return document_; }

    const DocCursor &normalCursor() { return m_normalCursor; }

    RowN lastActRow() { return m_lastActRow; }

    void setNormalCursor(const DocLoc &loc);

    void setNormalCursor(const DocCursor &cursor);

    // 以字符为单位，获得向左移动光标时的下一个字符位置
    DocLoc getNextLeftLocByChar(const DocLoc & loc) const;

    // 以字符为单位，获得向右移动光标时的下一个字符位置
    DocLoc getNextRightLocByChar(const DocLoc & loc) const;

public:
    auto &sigLastActRowUpdated() {
        return sigLastActRowUpdated_;
    }

private:
    void setLastActRow(RowN row);

private:
    doc::Document document_;

    // 普通模式光标
    DocCursor m_normalCursor;

    // 最后一次活跃的行
    RowN m_lastActRow = -1;

private:
    Signal<void()> sigLastActRowUpdated_;
};
