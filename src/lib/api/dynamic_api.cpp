#include "dynamic_api.h"

#include <cstring>
#include <string>
#include <boost/dll.hpp>

#include "charset/to_utf32.h"
#include "qt/application.h"


namespace
{

struct LitextApi {
    InitInfo initInfo;
    gui::qt::Application application;
};

inline LitextApi *asLitext(void *p) {
    return reinterpret_cast<LitextApi *>(p);
}

void *create()
{
    return new LitextApi;
}

void destroy(void *p)
{
    delete asLitext(p);
}

void initSetShouldStartAsServer(void *p, int b)
{
    asLitext(p)->initInfo.setShouldStartAsServer(b);
}

void initSetLogLevel(void *p, const char *level)
{
    asLitext(p)->initInfo.setLogLevel(level);
}

void initSetShouldLogToStd(void *p, int b)
{
    asLitext(p)->initInfo.setShouldLogToStd(b);
}

void initAddOpenFileWithUtf8FilePathAndRowNum(void *p, const char *file, long long row)
{
    const fs::path path = static_cast<std::u32string &&>(u32str(u8str(file)));
    asLitext(p)->initInfo.addOpenInfo(path, row);
}

void initSetStyleSheetFilePathByUtf8(void *p, const char *file)
{
    const fs::path path = static_cast<std::u32string &&>(u32str(u8str(file)));
    asLitext(p)->initInfo.setStyleSheetFile(path);
}

void init(void *p)
{
    LitextApi *api = asLitext(p);
    api->application.init(api->initInfo);
}

int exec(void *p)
{
    return asLitext(p)->application.exec();
}

}


extern "C" BOOST_SYMBOL_EXPORT
void fillBodyApiTable(BodyApiTable *table)
{
    if (!table) {
        return;
    }

    table->create = create;
    table->destroy = destroy;
    table->initSetShouldStartAsServer = initSetShouldStartAsServer;
    table->initSetLogLevel = initSetLogLevel;
    table->initSetShouldLogToStd = initSetShouldLogToStd;
    table->initAddOpenFileWithUtf8FilePathAndRowNum = initAddOpenFileWithUtf8FilePathAndRowNum;
    table->initSetStyleSheetFilePathByUtf8 = initSetStyleSheetFilePathByUtf8;
    table->init = init;
    table->exec = exec;
}
