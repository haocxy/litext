#include "my_widget.h"

#include <QEvent>
#include "run_on_qobject_event.h"


namespace gui::qt
{


bool MyWidget::event(QEvent *ev) {
	if (ev->type() == RunOnQObjectEvent::g_type) {
		RunOnQObjectEvent *e = static_cast<RunOnQObjectEvent *>(ev);
		e->call();
		return true;
	}

	return QWidget::event(ev);
}


}
