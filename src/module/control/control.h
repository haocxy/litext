#pragma once

#include <vector>


class Select;

/*
������
�����ṩ�����ĵ����ݵĽӿ�
�磬ѡ���ĵ���ĳЩ�ַ�������ӿ�֧��
*/
class Control
{
public:
    virtual ~Control() {}

    

    virtual const std::vector<Select> &GetAllSelect() const = 0;
};
