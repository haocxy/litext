#include "skip_row.h"

#include <string>


namespace
{

using byte = unsigned char;

using bytes = std::basic_string<byte>;

}


namespace doc
{

static void skipRowForAscii(std::istream &in, bytes &skiped) {
    constexpr char eof = std::istream::traits_type::eof();
    char ch = 0;
    while ((ch = in.get()) != eof) {
        skiped.push_back(ch);
        if (ch == '\r') {
            char next = 0;
            if ((next = in.get()) != eof) {
                if (next == '\n') {
                    skiped.push_back(next);
                    return;
                } else {
                    in.unget();
                    continue;
                }
            } else {
                return;
            }
        } else if (ch == '\n') {
            return;
        } else {
            continue;
        }
    }
}

static void skipRowForUnknown(std::istream &in, bytes &skiped) {
    skipRowForAscii(in, skiped);
}

// 把文件流ifs定位到换行字节组之后的下一个字节
static void skipRow(std::istream &in, Charset charset, bytes &skiped) {
    switch (charset) {
    case Charset::Ascii:
    case Charset::UTF_8:
    case Charset::GB18030:
        skipRowForAscii(in, skiped);
        return;
    default:
        skipRowForUnknown(in, skiped);
        return;
    }
}


void skipRow(Charset charset, std::istream &in, MemBuff &buff)
{
    bytes skiped;
    skipRow(in, charset, skiped);
    if (!skiped.empty()) {
        buff.append(skiped.data(), skiped.size());
    }
}

}

