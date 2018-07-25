#pragma once

#include "doc_define.h"

class DocLine;

// �ĵ����ṩ�Բ�ͬ���͡���ͬ��С���ĵ��ĳ���
// �ṩ�͵ײ���ʵ���ݵĽ���
class Doc
{
public:
    virtual ~Doc() {}

    virtual LineN lineCnt() const = 0;

    virtual const DocLine &lineAt(LineN line) const = 0;
};
