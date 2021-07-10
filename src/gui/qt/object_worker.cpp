#include "object_worker.h"

#include <QCoreApplication>

#include "run_in_gui_thread_event.h"


namespace gui::qt
{


void GuiThreadWorker::post(std::function<void()> &&action)
{
	QCoreApplication::sendEvent(&receiver_, new RunInGuiThreadEvent(std::move(action)));
}


}
