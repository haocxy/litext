#include "dynamic_api.h"

#include <cstring>
#include <string>
#include <boost/dll.hpp>
#include <boost/preprocessor/cat.hpp>

#include "charset/to_utf32.h"
#include "entry/init_info.h"
#include "entry/entry.h"


#define NOTESHARP_API_EXPORT extern "C" BOOST_SYMBOL_EXPORT

#define NOTESHARP_API_FUNC(f) \
NOTESHARP_API_EXPORT BOOST_PP_CAT(NOTESHARP_API_FR_, f)() \
BOOST_PP_CAT(NOTESHARP_API_FN_, f)() \
BOOST_PP_CAT(NOTESHARP_API_FP_, f)() \


namespace
{

struct NoteSharpApi {
    InitInfo initInfo;
};

inline NoteSharpApi *asNoteSharp(void *p) {
    return reinterpret_cast<NoteSharpApi *>(p);
}

}


NOTESHARP_API_FUNC(create)
{
    return new NoteSharpApi;
}

NOTESHARP_API_FUNC(destroy)
{
    delete asNoteSharp(p);
}

NOTESHARP_API_FUNC(initSetShouldStartAsServer)
{
    asNoteSharp(p)->initInfo.setShouldStartAsServer(b);
}

NOTESHARP_API_FUNC(initSetLogLevel)
{
    asNoteSharp(p)->initInfo.setLogLevel(level);
}

NOTESHARP_API_FUNC(initSetShouldLogToStd)
{
    asNoteSharp(p)->initInfo.setShouldLogToStd(b);
}

NOTESHARP_API_FUNC(initAddOpenFileWithUtf8FilePathAndRowNum)
{
    const std::u32string u32s = charset::toUTF32(Charset::UTF_8, file, std::strlen(file));
    const fs::path path = u32s;
    asNoteSharp(p)->initInfo.addOpenInfo(path, row);
}

NOTESHARP_API_FUNC(exec)
{
    return entry(asNoteSharp(p)->initInfo);
}



