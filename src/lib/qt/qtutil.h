#pragma once

#include <set>

#include <QString>
#include <QList>


namespace gui::qt::util
{

QList<QString> toQList(const std::set<QString> &set);

std::set<QString> supportedCharsetSet();

inline QList<QString> supportedCharsetList()
{
    return toQList(supportedCharsetSet());
}

}
