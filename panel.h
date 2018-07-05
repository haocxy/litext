#pragma once

#include <vector>

#include "docmodel.h"

class Panel
{
public:
    Panel(DocModel & model);

private:
    DocModel & m_model;

private:
    std::vector<DocSel> m_sels;
};
