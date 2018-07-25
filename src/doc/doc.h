#pragma once

#include "doc_define.h"

class DocLine;

// 文档，提供对不同类型、不同大小的文档的抽象
// 提供和底层真实数据的交互
class Doc
{
public:
    virtual ~Doc() {}

    virtual LineN lineCnt() const = 0;

    virtual const DocLine &lineAt(LineN line) const = 0;
};
