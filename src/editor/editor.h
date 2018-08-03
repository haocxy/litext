#pragma once

#include <vector>
#include "cursor.h"
#include "util/listeners.h"
#include "common/dir_enum.h"

class Doc;

/*
逻辑上操作文档
用于提供操作文档数据的接口
如，选择文档中某些字符由这个接口支持
*/
class Editor
{
public:
    Editor(Doc * model);

    const Doc & doc() const { return m_model; }

    const DocCursor &normalCursor() { return m_normalCursor; }

    LineN lastActLine() { return m_lastActLine; }

    void onPrimaryKeyPress(const DocAddr &addr);

    void setNormalCursor(const DocCursor & cursor);

    // 以字符为单位，获得向左移动光标时的下一个字符位置
    DocAddr getNextLeftAddrByChar(const DocAddr & addr) const;

    // 以字符为单位，获得向右移动光标时的下一个字符位置
    DocAddr getNextRightAddrByChar(const DocAddr & addr) const;

public:
    ListenerID addOnLastActLineUpdateListener(std::function<void()> && action);
    void removeOnLastActLineUpdateListener(ListenerID id);

private:
    void setLastActLine(LineN line);

private:

    Doc & m_model;

    // 普通模式光标
    DocCursor m_normalCursor;

    // 最后一次活跃的行
    LineN m_lastActLine = -1;

private:
    Listeners<void()> m_onLastActLineUpdateListeners;


};
