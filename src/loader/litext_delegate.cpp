#include "litext_delegate.h"

#include <boost/dll/shared_library.hpp>
#include <boost/dll/runtime_symbol_info.hpp>

#ifdef WIN32
#define LITEXT_DLIB_NAME "litext_body.dll"
#else
#define LITEXT_DLIB_NAME "liblitext_body.so"
#endif

LitextDelegate::Factory::Factory() {
    const auto exePath = boost::dll::program_location();
    const auto dlibPath = exePath.parent_path() / LITEXT_DLIB_NAME;
    dll_ = boost::dll::shared_library(dlibPath);
    fnTable_.load(dll_);
}
