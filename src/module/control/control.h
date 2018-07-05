#pragma once

#include <vector>


class Select;

/*
控制器
用于提供操作文档数据的接口
如，选择文档中某些字符由这个接口支持
*/
class Control
{
public:
    virtual ~Control() {}

    

    virtual const std::vector<Select> &GetAllSelect() const = 0;
};
