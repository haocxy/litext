#pragma once

#include <QString>
#include <QtGlobal>

namespace FileUtil
{
#ifdef Q_OS_WIN
    static const char *kDefaultEncoding = "gb18030";
#else
    static const char *kDefaultEncoding = "utf-8";
#endif

    class Err
    {
    public:
        Err(const QString &msg) : m_msg(msg) {}
        
        template <typename T>
        Err & operator,(const T &obj)
        {
            m_msg.arg(obj); return *this; 
        }
        const QString &GetMsg() const { return m_msg; }
    private:
        QString m_msg;
    };

    QString ReadFile(const QString &file, const char *encoding = kDefaultEncoding);
}
