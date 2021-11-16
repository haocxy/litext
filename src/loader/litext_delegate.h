#pragma once

#include <cstdint>
#include <filesystem>

#include <boost/dll/shared_library.hpp>

#include "core/fs.h"
#include "api/dynamic_api.h"


class LitextDelegate {
public:
    using FnTable = BodyApiTable;

public:
    class Factory {
    public:
        Factory();

    private:
        boost::dll::shared_library dll_;
        FnTable fnTable_;

        friend class LitextDelegate;
    };

public:

    LitextDelegate(Factory &factory)
        : fnTable_(factory.fnTable_) {

        handle_ = fnTable_.create();
    }

    LitextDelegate() = delete;

    LitextDelegate(const LitextDelegate &) = delete;

    LitextDelegate(LitextDelegate &&) = delete;

    LitextDelegate &operator=(const LitextDelegate &) = delete;

    LitextDelegate &operator=(LitextDelegate &&) = delete;

    ~LitextDelegate() {
        fnTable_.destroy(handle_);
        handle_ = nullptr;
    }

    void initSetShouldStartAsServer(bool b) {
        fnTable_.initSetShouldStartAsServer(handle_, b);
    }

    void initSetLogLevel(const char *level) {
        fnTable_.initSetLogLevel(handle_, level);
    }

    void initSetShouldLogToStd(bool b) {
        fnTable_.initSetShouldLogToStd(handle_, b);
    }

    void initAddOpenInfo(const std::filesystem::path &file, int64_t row) {
        fnTable_.initAddOpenFileWithUtf8FilePathAndRowNum(handle_, file.generic_u8string().c_str(), row);
    }

    void initSetStyleSheetFile(const fs::path &file) {
        fnTable_.initSetStyleSheetFilePathByUtf8(handle_, file.generic_u8string().c_str());
    }

    void init() {
        fnTable_.init(handle_);
    }

    int exec() {
        return fnTable_.exec(handle_);
    }

private:
    const FnTable &fnTable_;
    void *handle_ = nullptr;
};
