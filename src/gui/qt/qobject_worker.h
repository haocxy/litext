#pragma once

#include "core/worker.h"


class QObject;


namespace gui::qt
{


class QObjectWorker : public Worker {
public:
	QObjectWorker(QObject &receiver)
		: receiver_(receiver) {}

	virtual ~QObjectWorker() {}

	virtual void post(std::function<void()> &&action) override;

private:
	QObject &receiver_;
};


}
