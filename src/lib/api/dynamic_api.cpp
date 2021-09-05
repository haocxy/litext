#include "dynamic_api.h"

#include <cstring>
#include <string>
#include <boost/dll.hpp>
#include <boost/preprocessor/cat.hpp>

#include "charset/to_utf32.h"
#include "gui/qt/application.h"


#define LITEXT_API_EXPORT extern "C" BOOST_SYMBOL_EXPORT

#define LITEXT_API_FUNC_IMPL(f) \
LITEXT_API_EXPORT BOOST_PP_CAT(LITEXT_API_FR_, f)() \
BOOST_PP_CAT(LITEXT_API_FN_, f)() \
BOOST_PP_CAT(LITEXT_API_FP_, f)() \


namespace
{

struct LitextApi {
    InitInfo initInfo;
};

inline LitextApi *asLitext(void *p) {
    return reinterpret_cast<LitextApi *>(p);
}

}


LITEXT_API_FUNC_IMPL(create)
{
    return new LitextApi;
}

LITEXT_API_FUNC_IMPL(destroy)
{
    delete asLitext(p);
}

LITEXT_API_FUNC_IMPL(initSetShouldStartAsServer)
{
    asLitext(p)->initInfo.setShouldStartAsServer(b);
}

LITEXT_API_FUNC_IMPL(initSetLogLevel)
{
    asLitext(p)->initInfo.setLogLevel(level);
}

LITEXT_API_FUNC_IMPL(initSetShouldLogToStd)
{
    asLitext(p)->initInfo.setShouldLogToStd(b);
}

LITEXT_API_FUNC_IMPL(initAddOpenFileWithUtf8FilePathAndRowNum)
{
    const std::u32string u32s = charset::toUTF32(Charset::UTF_8, file, std::strlen(file));
    const fs::path path = u32s;
    asLitext(p)->initInfo.addOpenInfo(path, row);
}

LITEXT_API_FUNC_IMPL(exec)
{
    gui::qt::Application app(asLitext(p)->initInfo);
    return app.exec();
}
