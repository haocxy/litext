#include "doc_controller.h"

#include <assert.h>
#include "module/model/model.h"
#include "module/model/line.h"

DocController::DocController(Model * model)
    : m_model(*model)
{
    assert(model);
}







