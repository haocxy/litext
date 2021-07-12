#pragma once

#include <string>

#include "uchar.h"


typedef std::basic_string<UChar> UString;

class QString;


namespace UStringUtil
{

UString toUString(const QString &str);

}
