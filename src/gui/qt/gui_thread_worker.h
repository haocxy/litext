#pragma once

#include <QObject>

#include "core/worker.h"



namespace gui::qt
{


class GuiThreadWorker : public QObject, public Worker {
	Q_OBJECT
public:
	GuiThreadWorker();

	virtual ~GuiThreadWorker() {}

	virtual void post(std::function<void()> &&action) override;

private slots:
	void runMyFunction(void *ptr);

signals:
	void postMyFunction(void *ptr);
};


}
