#pragma once

#include <QChar>

class CharInStream
{
public:
    virtual ~CharInStream() {}

    virtual QChar Next() = 0;
};
