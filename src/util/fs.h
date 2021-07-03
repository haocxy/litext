#pragma once

#if __has_include(<filesystem>)
#include <filesystem>
#elif __has_include(<experimental/filesystem>)
#include <experimental/filesystem>
#else
#error "Cannot find <filesystem> header file"
#endif

namespace fs
{
using namespace std::filesystem;
}
