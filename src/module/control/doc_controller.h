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

private:

    Model & m_model;

    // ��ͨģʽ���
    DocCursor m_normalCursor;
};
