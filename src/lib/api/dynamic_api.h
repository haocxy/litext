#pragma once


// 通过如下导出函数填充函数表
// extern "C" void fillBodyApiTable(BodyApiTable *table);
extern "C" struct BodyApiTable {

    using Create = void *(*)();
    Create create = nullptr;

    using Destroy = void(*)(void *);
    Destroy destroy = nullptr;

    using InitSetShouldStartAsServer = void(*)(void *p, int b);
    InitSetShouldStartAsServer initSetShouldStartAsServer = nullptr;

    using InitSetLogLevel = void(*)(void *p, const char *level);
    InitSetLogLevel initSetLogLevel = nullptr;

    using InitSetShouldLogToStd = void(*)(void *p, int b);
    InitSetShouldLogToStd initSetShouldLogToStd = nullptr;

    using InitAddOpenFileWithUtf8FilePathAndRowNum = void(*)(void *p, const char *file, long long row);
    InitAddOpenFileWithUtf8FilePathAndRowNum initAddOpenFileWithUtf8FilePathAndRowNum = nullptr;

    using InitSetStyleSheetFilePathByUtf8 = void(*)(void *p, const char *file);
    InitSetStyleSheetFilePathByUtf8 initSetStyleSheetFilePathByUtf8 = nullptr;

    using Init = void(*)(void *p);
    Init init = nullptr;

    using Exec = int(*)(void *p);
    Exec exec = nullptr;
};


