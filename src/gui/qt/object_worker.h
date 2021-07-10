#pragma once

#include "core/worker.h"


class QObject;


namespace gui::qt
{


class GuiThreadWorker : public Worker {
public:
	GuiThreadWorker(QObject &receiver)
		: receiver_(receiver) {}

	virtual ~GuiThreadWorker() {}

	virtual void post(std::function<void()> &&action) override;

private:
	QObject &receiver_;
};


}
