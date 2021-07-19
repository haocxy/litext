#pragma once

#include <functional>

#include "core/ustring.h"
#include "text/char_instream.h"

#include "declare_text_area_config.h"
#include "view_define.h"
#include "view_char.h"


namespace gui
{

class RowWalker {
public:
    using CharOperation = std::function<void(bool isEmptyRow, size_t lineIndex, const VChar &vchar)>;

    RowWalker(const TextAreaConfig &config, int widthLimit, CharInStream &charStream);

    void forEachChar(CharOperation &&operation);

private:
    const TextAreaConfig &config_;
    const int widthLimit_;
    CharInStream &charStream_;
};


}
