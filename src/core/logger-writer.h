#pragma once

#include <ctime>
#include <stdexcept>
#include <string>
#include <fstream>

#include "fs.h"


namespace logger
{

class Writer
{
public:

    class PathCheckError : public std::logic_error
    {
    public:
        PathCheckError(const char *msg) : std::logic_error(msg) {}
        virtual ~PathCheckError() {}
    };

    Writer(const fs::path &dir, const std::string &basename);

    void write(const void *data, size_t len);

    void flush() {
        out_.flush();
    }

private:
    const fs::path base_;
    std::ofstream out_;
    std::tm lastTime_;
};

} // namespace logger
