#pragma once

#include <vector>
#include "doc_cursor.h"

class Model;

/*
������
�����ṩ�����ĵ����ݵĽӿ�
�磬ѡ���ĵ���ĳЩ�ַ�������ӿ�֧��
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

    // ��ͨģʽ���
    DocCursor m_normalCursor;

    // ���һ�λ�Ծ����
    LineN m_lastActLine;
};
