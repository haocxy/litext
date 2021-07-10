#include "run_on_qobject_event.h"


namespace gui::qt
{

QEvent::Type RunInGuiThreadEvent::g_type = QEvent::None;



}
