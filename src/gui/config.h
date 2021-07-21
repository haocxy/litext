#pragma once

#include "declare_text_area_config.h"


namespace gui
{

class Config {
public:
    Config();

    ~Config();

    const TextAreaConfig &textAreaConfig() const {
        return *textAreaConfig_;
    }

    TextAreaConfig &textAreaConfig() {
        return *textAreaConfig_;
    }

private:
    TextAreaConfig *textAreaConfig_ = nullptr;
};

}


