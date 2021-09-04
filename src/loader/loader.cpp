#include <boost/dll/shared_library.hpp>
#include <boost/dll/runtime_symbol_info.hpp>

#include "notesharp_delegate.h"

#ifdef WIN32
#define NOTESHARP_DLIB_NAME "libnotesharp.dll"
#else
#define NOTESHARP_DLIB_NAME "liblibnotesharp.so"
#endif

int loader_entry(int argc, char *argv[])
{
    const auto exePath = boost::dll::program_location();
    const auto dlibPath = exePath.parent_path() / NOTESHARP_DLIB_NAME;
    NoteSharpDelegate::Factory factory(dlibPath);
    NoteSharpDelegate notesharp(factory);
    return notesharp.exec();
}
