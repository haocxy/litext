#pragma once

#include "model_define.h"

class Line;

// 文档模型
// 提供和底层真实数据的交互
class Model
{
public:
    virtual ~Model() {}

    virtual LineN lineCnt() const = 0;

    virtual const Line &lineAt(LineN line) const = 0;
};
