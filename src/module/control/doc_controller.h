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

private:

    Model & m_model;

    // 普通模式光标
    DocCursor m_normalCursor;
};
