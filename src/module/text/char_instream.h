#pragma once

#include <QChar>

class CharInStream
{
public:
    virtual ~CharInStream() {}

    virtual bool HasNext() = 0;

    virtual QChar Next() = 0;
};
