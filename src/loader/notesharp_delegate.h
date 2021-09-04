#pragma once

#include <cstdint>
#include <filesystem>

#include <boost/dll/shared_library.hpp>

#include "api/dynamic_api.h"


// 获得C字符串形式的实际函数名
#define LITEXT_API_FN_NAME(f) BOOST_PP_STRINGIZE(BOOST_PP_CAT(LITEXT_API_FN_,f)())


// 获得函数类型
#define LITEXT_API_FN_TYPE(f) BOOST_PP_CAT(LITEXT_API_FR_, f)() BOOST_PP_CAT(LITEXT_API_FP_, f)()



class NoteSharpDelegate {
public:

    struct FnTable {
        using Create = LITEXT_API_FN_TYPE(create);
        using Destroy = LITEXT_API_FN_TYPE(destroy);
        using InitSetShouldStartAsServer = LITEXT_API_FN_TYPE(initSetShouldStartAsServer);
        using InitSetLogLevel = LITEXT_API_FN_TYPE(initSetLogLevel);
        using InitAddOpenFileWithUtf8FilePathAndRowNum = LITEXT_API_FN_TYPE(initAddOpenFileWithUtf8FilePathAndRowNum);
        using Exec = LITEXT_API_FN_TYPE(exec);

        Create *create = nullptr;
        Destroy *destroy = nullptr;
        InitSetShouldStartAsServer *initSetShouldStartAsServer = nullptr;
        InitSetLogLevel *initSetLogLevel = nullptr;
        InitAddOpenFileWithUtf8FilePathAndRowNum *initAddOpenFileWithUtf8FilePathAndRowNum = nullptr;
        Exec *exec = nullptr;

        void load(boost::dll::shared_library &dll) {
            create = dll.get<Create>(LITEXT_API_FN_NAME(create));
            destroy = dll.get<Destroy>(LITEXT_API_FN_NAME(destroy));
            initSetShouldStartAsServer = dll.get<InitSetShouldStartAsServer>(LITEXT_API_FN_NAME(initSetShouldStartAsServer));
            initSetLogLevel = dll.get<InitSetLogLevel>(LITEXT_API_FN_NAME(initSetLogLevel));
            initAddOpenFileWithUtf8FilePathAndRowNum = dll.get<InitAddOpenFileWithUtf8FilePathAndRowNum>(LITEXT_API_FN_NAME(initAddOpenFileWithUtf8FilePathAndRowNum));
            exec = dll.get<Exec>(LITEXT_API_FN_NAME(exec));
        }
    };

public:
    class Factory {
    public:
        Factory(const boost::filesystem::path &dllFile) {
            dll_ = boost::dll::shared_library(dllFile);
            fnTable_.load(dll_);
        }

    private:
        boost::dll::shared_library dll_;
        FnTable fnTable_;

        friend class NoteSharpDelegate;
    };

public:

    NoteSharpDelegate(Factory &factory)
        : fnTable_(factory.fnTable_) {

        handle_ = fnTable_.create();
    }

    NoteSharpDelegate() = delete;

    NoteSharpDelegate(const NoteSharpDelegate &) = delete;

    NoteSharpDelegate(NoteSharpDelegate &&) = delete;

    NoteSharpDelegate &operator=(const NoteSharpDelegate &) = delete;

    NoteSharpDelegate &operator=(NoteSharpDelegate &&) = delete;

    ~NoteSharpDelegate() {
        fnTable_.destroy(handle_);
        handle_ = nullptr;
    }

    void initSetShouldStartAsServer(bool b) {
        fnTable_.initSetShouldStartAsServer(handle_, b);
    }

    void initSetLogLevel(const char *level) {
        fnTable_.initSetLogLevel(handle_, level);
    }

    void initAddOpenInfo(const std::filesystem::path &file, int64_t row) {
        fnTable_.initAddOpenFileWithUtf8FilePathAndRowNum(handle_, file.generic_u8string().c_str(), row);
    }

    int exec() {
        return fnTable_.exec(handle_);
    }

private:
    const FnTable &fnTable_;
    void *handle_ = nullptr;
};
