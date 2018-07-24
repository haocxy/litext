#include "doc_controller.h"

#include <assert.h>
#include "model/model.h"
#include "model/line.h"

DocController::DocController(Model * model)
    : m_model(*model)
{
    assert(model);
}

void DocController::onPrimaryKeyPress(const DocAddr & addr)
{
    m_normalCursor.setAddr(addr);

    if (!addr.isAfterLastPhase())
    {
        m_lastActLine = addr.line();
    }
}







