#include <iostream>

#include "litext_delegate.h"
#include "cmdopt.h"




int loader_entry(int argc, char *argv[])
{
    CmdOpt opt(argc, argv);
    if (opt.needHelp()) {
        opt.help(std::cout);
        return 0;
    }

    LitextDelegate::Factory factory;
    LitextDelegate litext(factory);

    litext.initSetLogLevel(opt.logLevel());
    litext.initSetShouldStartAsServer(true);
    for (const auto &pair : opt.files()) {
        litext.initAddOpenInfo(pair.first, pair.second);
    }

    litext.init();

    return litext.exec();
}
