#pragma once

#include <QObject>

#include "core/worker.h"


struct QtGuiFuncWrapper {
	std::function<void()> f;
};

Q_DECLARE_METATYPE(QtGuiFuncWrapper)


namespace gui::qt
{


class GuiThreadWorker : public QObject, public Worker {
	Q_OBJECT
public:
	GuiThreadWorker();

	virtual ~GuiThreadWorker() {}

	virtual void post(std::function<void()> &&action) override;

private slots:
	void runMyFunction(QtGuiFuncWrapper wrapper);

signals:
	void postMyFunction(QtGuiFuncWrapper wrapper);
};


}
