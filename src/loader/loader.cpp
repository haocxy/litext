#include <boost/dll/shared_library.hpp>
#include <boost/dll/runtime_symbol_info.hpp>

#include "litext_delegate.h"

#ifdef WIN32
#define LITEXT_DLIB_NAME "liblitext.dll"
#else
#define LITEXT_DLIB_NAME "libliblitext.so"
#endif

int loader_entry(int argc, char *argv[])
{
    const auto exePath = boost::dll::program_location();
    const auto dlibPath = exePath.parent_path() / LITEXT_DLIB_NAME;
    LitextDelegate::Factory factory(dlibPath);
    LitextDelegate litext(factory);
    return litext.exec();
}
