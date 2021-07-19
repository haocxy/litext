#include "gui_thread_worker.h"

#include <mutex>


namespace gui::qt
{

static std::once_flag g_RegisterOnceFlag;


GuiStrand::GuiStrand()
{
	std::call_once(g_RegisterOnceFlag, []() {
		qRegisterMetaType<QtGuiFuncWrapper>("QtGuiFuncWrapper");
	});

	QObject::connect(this, &GuiStrand::postMyFunction, this, &GuiStrand::runMyFunction);
}

void GuiStrand::post(std::function<void()> &&action)
{
	QtGuiFuncWrapper wrapper;
	wrapper.f = std::move(action);
	emit postMyFunction(wrapper);
}

void GuiStrand::runMyFunction(QtGuiFuncWrapper wrapper)
{
	if (wrapper.f) {
		wrapper.f();
	}
}

}
