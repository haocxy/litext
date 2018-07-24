#pragma once

#include "model_define.h"

class Line;

// �ĵ�ģ��
// �ṩ�͵ײ���ʵ���ݵĽ���
class Model
{
public:
    virtual ~Model() {}

    virtual LineN lineCnt() const = 0;

    virtual const Line &lineAt(LineN line) const = 0;
};
