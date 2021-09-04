#include "singleton_server.h"


namespace gui
{



SingletonServer::SingletonServer()
{
}

SingletonServer::~SingletonServer()
{
    if (started_) {
        context_.stop();
        stopping_ = true;
        thread_.join();
    }
}

void SingletonServer::start()
{
    if (started_) {
        return;
    }

    if (stopping_) {
        return;
    }

    started_ = true;

    thread_ = std::thread([this] {
        loop();
    });
}

void SingletonServer::loop()
{
    while (!stopping_) {
        context_.run_one();
    }
}

}
