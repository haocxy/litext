#pragma once

#include "core/worker.h"


class QObject;


namespace gui::qt
{


class ObjectWorker : public Worker {
public:
	ObjectWorker(QObject &receiver)
		: receiver_(receiver) {}

	virtual ~ObjectWorker() {}

	virtual void post(std::function<void()> &&action) override;

private:
	QObject &receiver_;
};


}
