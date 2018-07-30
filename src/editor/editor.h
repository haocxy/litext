#pragma once

#include <vector>
#include "cursor.h"

class Doc;

/*
�߼��ϲ����ĵ�
�����ṩ�����ĵ����ݵĽӿ�
�磬ѡ���ĵ���ĳЩ�ַ�������ӿ�֧��
*/
class Editor
{
public:
    Editor(Doc * model);

    const Doc & doc() const { return m_model; }

    DocCursor &normalCursor() { return m_normalCursor; }

    LineN lastActLine() { return m_lastActLine; }
    void setLastActLine(LineN line) { m_lastActLine = line; }

    void onPrimaryKeyPress(const DocAddr &addr);

private:

    Doc & m_model;

    // ��ͨģʽ���
    DocCursor m_normalCursor;

    // ���һ�λ�Ծ����
    LineN m_lastActLine;
};
