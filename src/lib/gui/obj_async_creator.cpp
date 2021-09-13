#include "obj_async_creator.h"

namespace gui
{

ObjAsyncCreator::ObjAsyncCreator()
    : creatables_()
    , thread_([this] { loop(); })
{

}

ObjAsyncCreator::~ObjAsyncCreator()
{
    stopping_ = true;
    creatables_.stop();
    thread_.join();
}

void ObjAsyncCreator::loop()
{
    while (!stopping_) {
        std::optional<std::unique_ptr<Creatable>> creatableOpt = creatables_.pop();
        if (creatableOpt) {
            const std::unique_ptr<Creatable> &creatablePtr = *creatableOpt;
            if (creatablePtr) {
                creatablePtr->create();
            }
        }
    }
}

}
