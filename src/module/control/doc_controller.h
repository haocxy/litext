#pragma once

#include <vector>
#include "doc_cursor.h"

class Model;

/*
控制器
用于提供操作文档数据的接口
如，选择文档中某些字符由这个接口支持
*/
class DocController
{
public:
    DocController(Model * model);

    DocCursor &normalCursor() { return m_normalCursor; }

    LineN lastActLine() { return m_lastActLine; }
    void setLastActLine(LineN line) { m_lastActLine = line; }

    void onPrimaryKeyPress(const DocAddr &addr);

private:

    Model & m_model;

    // 普通模式光标
    DocCursor m_normalCursor;

    // 最后一次活跃的行
    LineN m_lastActLine;
};
