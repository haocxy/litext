#include <iostream>
#include <boost/dll/shared_library.hpp>
#include <boost/dll/runtime_symbol_info.hpp>

#include "litext_delegate.h"
#include "cmdopt.h"


#ifdef WIN32
#define LITEXT_DLIB_NAME "litext_body.dll"
#else
#define LITEXT_DLIB_NAME "liblitext_body.so"
#endif

int loader_entry(int argc, char *argv[])
{
    const auto exePath = boost::dll::program_location();
    const auto dlibPath = exePath.parent_path() / LITEXT_DLIB_NAME;
    LitextDelegate::Factory factory(dlibPath);
    LitextDelegate litext(factory);
    CmdOpt opt(argc, argv);
    if (opt.needHelp()) {
        opt.help(std::cout);
        return 0;
    }

    litext.initSetLogLevel(opt.logLevel());
    litext.initSetShouldStartAsServer(true);
    for (const auto &pair : opt.files()) {
        litext.initAddOpenInfo(pair.first, pair.second);
    }

    litext.init();

    return litext.exec();
}
