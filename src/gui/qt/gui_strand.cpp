#include "gui_strand.h"

#include <mutex>

#include <QApplication>


namespace gui::qt
{

static std::once_flag g_RegisterOnceFlag;


GuiStrand::GuiStrand()
{
	std::call_once(g_RegisterOnceFlag, []() {
		qRegisterMetaType<QtGuiFuncWrapper>("QtGuiFuncWrapper");
	});

    moveToThread(QApplication::instance()->thread());

	QObject::connect(this, &GuiStrand::postMyFunction, this, &GuiStrand::runMyFunction);
}

void GuiStrand::post(std::function<void()> &&action)
{
	QtGuiFuncWrapper wrapper;
	wrapper.f = std::move(action);
	emit postMyFunction(wrapper);
}

bool GuiStrand::isStopping() const
{
	return false; // TODO 目前还不需要查询GUI线程正在停止，将来考虑下怎么处理
}

void GuiStrand::runMyFunction(QtGuiFuncWrapper wrapper)
{
	if (wrapper.f) {
		wrapper.f();
	}
}

}
