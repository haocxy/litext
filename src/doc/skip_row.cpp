#include "skip_row.h"

#include <string>


namespace
{

using byte = unsigned char;

using bytes = std::basic_string<byte>;

}


namespace doc
{

static bool skipRowForAscii(std::istream &in, bytes &skiped, i32 bytesLimit) {
    constexpr char eof = std::istream::traits_type::eof();
    char ch = 0;
    while ((ch = in.get()) != eof) {
        skiped.push_back(ch);
        if (skiped.size() > bytesLimit) {
            return false;
        }
        if (ch == '\r') {
            char next = 0;
            if ((next = in.get()) != eof) {
                if (next == '\n') {
                    skiped.push_back(next);
                    return true;
                } else {
                    in.unget();
                    continue;
                }
            } else {
                return true;
            }
        } else if (ch == '\n') {
            return true;
        } else {
            continue;
        }
    }

    return true;
}

static bool skipRowForUnknown(std::istream &in, bytes &skiped, i32 bytesLimit) {
    return skipRowForAscii(in, skiped, bytesLimit);
}

// 把文件流ifs定位到换行字节组之后的下一个字节
static bool skipRow(std::istream &in, Charset charset, bytes &skiped, i32 bytesLimit) {
    switch (charset) {
    case Charset::Ascii:
    case Charset::UTF_8:
    case Charset::GB18030:
        return skipRowForAscii(in, skiped, bytesLimit);
    default:
        return skipRowForUnknown(in, skiped, bytesLimit);
    }
}


bool skipRow(Charset charset, std::istream &in, MemBuff &buff, i32 bytesLimit)
{
    bytes skiped;
    const bool result = skipRow(in, charset, skiped, bytesLimit);
    if (!skiped.empty()) {
        buff.append(skiped.data(), skiped.size());
    }
    return result;
}

}

