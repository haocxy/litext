#include "ustr.h"

#include "charset/to_utf32.h"


u32str::u32str(const u8str &u8s)
    : std::u32string(charset::toUTF32(Charset::UTF_8, u8s.data(), u8s.size()))
{
}
