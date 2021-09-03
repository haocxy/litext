#include <boost/dll/shared_library.hpp>
#include <boost/dll/runtime_symbol_info.hpp>

#define NOTESHARP_DLIB_ENTRY_NAME "entry"

#ifdef WIN32
#define NOTESHARP_DLIB_NAME "libnotesharp.dll"
#else
#define NOTESHARP_DLIB_NAME "liblibnotesharp.so"
#endif

int loader_entry(int argc, char *argv[])
{
    const auto exePath = boost::dll::program_location();
    const auto dlibPath = exePath.parent_path() / NOTESHARP_DLIB_NAME;
    boost::dll::shared_library dlib(dlibPath);
    auto entry = dlib.get<int(int, char **)>(NOTESHARP_DLIB_ENTRY_NAME);
    if (entry) {
        entry(argc, argv);
    }
    return 0;
}
