#include "gui_thread_worker.h"

#include <memory>


namespace gui::qt
{


GuiThreadWorker::GuiThreadWorker()
{
	QObject::connect(this, &GuiThreadWorker::postMyFunction, this, &GuiThreadWorker::runMyFunction);
}

void GuiThreadWorker::post(std::function<void()> &&action)
{
	// 若要投递std::function<void()>类型需要有比较繁琐的注册逻辑，直接在堆内存拷贝函数对象并投递其指针
	emit postMyFunction(new std::function<void()>(std::move(action)));
}

void GuiThreadWorker::runMyFunction(void *ptr)
{
	std::unique_ptr<std::function<void()>> f(reinterpret_cast<std::function<void()> *>(ptr));
	if (f && (*f)) {
		(*f)();
	}
}

}
