#include "gui_thread_worker.h"

#include <mutex>


namespace gui::qt
{

static std::once_flag g_RegisterOnceFlag;


GuiThreadWorker::GuiThreadWorker()
{
	std::call_once(g_RegisterOnceFlag, []() {
		qRegisterMetaType<QtGuiFuncWrapper>("QtGuiFuncWrapper");
	});

	QObject::connect(this, &GuiThreadWorker::postMyFunction, this, &GuiThreadWorker::runMyFunction);
}

void GuiThreadWorker::post(std::function<void()> &&action)
{
	QtGuiFuncWrapper wrapper;
	wrapper.f = std::move(action);
	emit postMyFunction(wrapper);
}

void GuiThreadWorker::runMyFunction(QtGuiFuncWrapper wrapper)
{
	if (wrapper.f) {
		wrapper.f();
	}
}

}
