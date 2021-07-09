#include "qobject_worker.h"

#include <QCoreApplication>

#include "run_on_qobject_event.h"


namespace gui::qt
{


void QObjectWorker::post(std::function<void()> &&action)
{
	QCoreApplication::sendEvent(&receiver_, new RunOnQObjectEvent(std::move(action)));
}


}
