#pragma once

#include <vector>
#include "cursor.h"
#include "util/listeners.h"
#include "common/dir_enum.h"

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

    void onDirectionKeyPress(Dir dir);

public:
    ListenerID addOnLastActLineUpdateListener(std::function<void()> && action);
    void removeOnLastActLineUpdateListener(ListenerID id);

private:

    Doc & m_model;

    // ��ͨģʽ���
    DocCursor m_normalCursor;

    // ���һ�λ�Ծ����
    LineN m_lastActLine = -1;

private:
    Listeners<void()> m_onLastActLineUpdateListeners;


};
