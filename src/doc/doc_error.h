#pragma once

namespace doc
{

// 文档错误
enum class DocError {

    // 没有错误
    NoError = 0,

    // 段落过长
    RowTooBig,
};

}
