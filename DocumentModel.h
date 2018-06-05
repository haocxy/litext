#pragma once

#include <vector>
#include <QString>

class DocumentModel
{
public:
    DocumentModel();
    ~DocumentModel();

    void ParseStr(const QString &s);

    int64_t GetLineCnt() const;

    int64_t GetCharCntOfLine(int64_t lineIndex) const;

    int64_t GetViewLineBegin() const;

    int64_t GetViewLineCnt() const;

    int64_t GetViewCharCntOfLine(int64_t viewLineIndex) const;

    QChar GetCharByViewPos(int64_t row, int64_t col) const;

    bool IsSelectedByViewPos(int64_t row, int64_t col) const;

private:
    std::vector<QString> lines_;
    int64_t view_line_begin_ = 0;
};

