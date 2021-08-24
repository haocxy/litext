#include "config.h"

#include "text_area_config.h"


namespace gui
{

Config::Config()
{
    textAreaConfig_ = new TextAreaConfig;
}

Config::~Config()
{
    delete textAreaConfig_;
    textAreaConfig_ = nullptr;
}

}
