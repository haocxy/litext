#include "cut_rows.h"

#include <cassert>

#include "core/basetype.h"
#include "core/uchar.h"


namespace text
{

scc::vector<UTF16Row> cutRows(const scc::u16string &s)
{
    enum class State {
        Normal,
        MeetCR, // '\r'
    };

    scc::vector<UTF16Row> result(s.get_allocator());
    State st = State::Normal;
    scc::u16string buff(s.get_allocator());
    bool partHasSuggorate = false;

    const i64 u16Len = s.size();

    for (i64 i = 0; i < u16Len; ++i) {
        const char16_t cur16ch = s[i];
        const bool curIsSuggorate = utf16::isSuggorate(cur16ch);
        char16_t next16ch = 0;
        char32_t unicode = 0;
        
        if (!curIsSuggorate) {
            unicode = cur16ch;
        } else {
            next16ch = s[i++];
            unicode = utf16::toUnicode(cur16ch, next16ch);
        }

        switch (st) {
        case State::Normal:
            if (unicode != '\r' && unicode != '\n') {
                if (!curIsSuggorate) {
                    buff.push_back(cur16ch);
                } else {
                    buff.push_back(cur16ch);
                    buff.push_back(next16ch);
                    partHasSuggorate = true;
                }
            } else if (unicode == '\r') {
                st = State::MeetCR;
            } else { // unicode == '\n'
                result.push_back(UTF16Row(RowEnd::LF, partHasSuggorate, std::move(buff)));
                partHasSuggorate = false;
            }
            break;
        case State::MeetCR:
            if (unicode != '\r' && unicode != '\n') {
                result.push_back(UTF16Row(RowEnd::CR, partHasSuggorate, std::move(buff)));
                if (!curIsSuggorate) {
                    buff.push_back(cur16ch);
                    partHasSuggorate = false;
                } else {
                    buff.push_back(cur16ch);
                    buff.push_back(next16ch);
                    partHasSuggorate = true;
                }
            } else if (unicode == '\r') {
                result.push_back(UTF16Row(RowEnd::CR, partHasSuggorate, std::move(buff)));
                partHasSuggorate = false;
            } else { // unicode == '\n'
                result.push_back(UTF16Row(RowEnd::CRLF, partHasSuggorate, std::move(buff)));
                partHasSuggorate = false;
                st = State::Normal;
            }
            break;
        default:
            break;
        }
    }

    if (!buff.empty()) {
        assert(buff.back() != '\n');
        // 如果buff中还有数据最后一个不可能是 '\n'
        // 因为前面的逻辑中只要遇到 '\n' 就会清空 buff
        switch (st) {
        case State::Normal:
            result.push_back(UTF16Row(RowEnd::NoEnd, partHasSuggorate, std::move(buff)));
            break;
        case State::MeetCR:
            result.push_back(UTF16Row(RowEnd::CR, partHasSuggorate, std::move(buff)));
            break;
        default:
            break;
        }
    }

    return result;
}

}
