#pragma once

#include <vector>
#include "cursor.h"
#include "util/callbacks.h"
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
    explicit Editor(Doc * model);

    const Doc & doc() const { return m_model; }

    const DocCursor &normalCursor() { return m_normalCursor; }

    RowN lastActRow() { return m_lastActRow; }

    void onPrimaryKeyPress(const DocLoc & loc);

    void setNormalCursor(const DocLoc & loc);

    void setNormalCursor(const DocCursor & cursor);

    // ���ַ�Ϊ��λ����������ƶ����ʱ����һ���ַ�λ��
    DocLoc getNextLeftLocByChar(const DocLoc & loc) const;

    // ���ַ�Ϊ��λ����������ƶ����ʱ����һ���ַ�λ��
    DocLoc getNextRightLocByChar(const DocLoc & loc) const;

public:
    CallbackHandle addOnLastActRowUpdateListener(std::function<void()> && action);

private:
    void setLastActRow(RowN row);

private:

    Doc & m_model;

    // ��ͨģʽ���
    DocCursor m_normalCursor;

    // ���һ�λ�Ծ����
    RowN m_lastActRow = -1;

private:
    Callbacks<void()> m_lastActRowUpdateListeners;
};
