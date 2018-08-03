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

    const DocCursor &normalCursor() { return m_normalCursor; }

    RowN lastActRow() { return m_lastActRow; }

    void onPrimaryKeyPress(const DocAddr &addr);

    void setNormalCursor(const DocCursor & cursor);

    // ���ַ�Ϊ��λ����������ƶ����ʱ����һ���ַ�λ��
    DocAddr getNextLeftAddrByChar(const DocAddr & addr) const;

    // ���ַ�Ϊ��λ����������ƶ����ʱ����һ���ַ�λ��
    DocAddr getNextRightAddrByChar(const DocAddr & addr) const;

public:
    ListenerID addOnLastActRowUpdateListener(std::function<void()> && action);
    void removeOnLastActRowUpdateListener(ListenerID id);

private:
    void setLastActRow(RowN row);

private:

    Doc & m_model;

    // ��ͨģʽ���
    DocCursor m_normalCursor;

    // ���һ�λ�Ծ����
    RowN m_lastActRow = -1;

private:
    Listeners<void()> m_lastActRowUpdateListeners;


};
