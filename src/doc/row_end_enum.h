#pragma once

enum class RowEnd
{
    NO   = 0, // 没有结束符，只存在于最后一行
    CR   = 1, // 以 \r 结束
    LF   = 2, // 以 \n 结束
    CRLF = 3, // 以 \r\n 结束
};
