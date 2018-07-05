#include "view.h"

void View::Init(DocPhaseN viewStart, const view::Size & size)
{
    m_viewStart = viewStart;
    m_size = size;


}

const view::Page & View::page() const
{
    return m_page;
}

void View::remakePage()
{
    m_page.clear();


}
