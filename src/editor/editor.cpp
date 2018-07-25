#include "editor.h"

#include <assert.h>
#include "doc/doc.h"
#include "doc/doc_line.h"

Editor::Editor(Doc * model)
    : m_model(*model)
{
    assert(model);
}

void Editor::onPrimaryKeyPress(const DocAddr & addr)
{
    m_normalCursor.setAddr(addr);

    if (addr.isAfterLastPhase())
    {
        m_lastActLine = m_model.lineCnt();
    }
    else
    {
        m_lastActLine = addr.line();
    }
}







