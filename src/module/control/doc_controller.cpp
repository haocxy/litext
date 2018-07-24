#include "doc_controller.h"

#include <assert.h>
#include "module/model/model.h"
#include "module/model/line.h"

DocController::DocController(Model * model)
    : m_model(*model)
{
    assert(model);
}

void DocController::onPrimaryKeyPress(const DocAddr & addr)
{
    m_normalCursor.setAddr(addr);
    m_lastActLine = addr.line();
}







