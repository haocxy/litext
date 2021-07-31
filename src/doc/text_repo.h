#pragma once

#include "core/sqlite.h"


namespace doc
{

// thread safe
class TextRepo {
public:
    TextRepo(const fs::path &dbFile);

    ~TextRepo();

    // 保存一段文本
    class SavePartStmt {
    public:
        SavePartStmt(TextRepo &repo);
        
        // 不区分文本的编码，原样处理二进制数据，编码由上层逻辑确定
        // off: 文本段在文档中的偏移量
        // nrows: 文本段的段落数
        // nlines: 文本段的行数
        // data: 文本数据
        // nbytes: 文本数据字节数
        // 返回由数据库生成的文本段ID
        i64 operator()(i64 off, i64 nrows, i64 nlines, const void *data, i64 nbytes);

    private:
        sqlite::Statement stmt_;
    };

private:
    bool shouldClearDb();

    void clearDb();

private:
    const fs::path &dbFile_;
    sqlite::Database db_;
    sqlite::Statement stSavePart_;
};

}
