#include <boost/dll/shared_library.hpp>
#include <boost/dll/runtime_symbol_info.hpp>

#include "litext_delegate.h"

#ifdef WIN32
#define NOTESHARP_DLIB_NAME "liblitext.dll"
#else
#define NOTESHARP_DLIB_NAME "libliblitext.so"
#endif

int loader_entry(int argc, char *argv[])
{
    const auto exePath = boost::dll::program_location();
    const auto dlibPath = exePath.parent_path() / NOTESHARP_DLIB_NAME;
    NoteSharpDelegate::Factory factory(dlibPath);
    NoteSharpDelegate notesharp(factory);
    return notesharp.exec();
}
