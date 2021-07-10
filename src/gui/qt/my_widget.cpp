#include "run_in_gui_thread_event_receiver.h"

#include <QEvent>

#include "run_in_gui_thread_event.h"


namespace gui::qt
{


bool RunInGuiThreadEventReceiver::event(QEvent *ev) {
	if (ev->type() == RunInGuiThreadEvent::g_type) {
		RunInGuiThreadEvent *e = static_cast<RunInGuiThreadEvent *>(ev);
		e->call();
		return true;
	} else {
		return false;
	}
}


}
