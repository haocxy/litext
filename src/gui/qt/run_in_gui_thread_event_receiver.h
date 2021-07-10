#pragma once

#include <QObject>


namespace gui::qt
{

class RunInGuiThreadEventReceiver : public QObject {
	Q_OBJECT
public:
	explicit RunInGuiThreadEventReceiver(QObject *parent = nullptr) : QObject(parent) {}

	virtual ~RunInGuiThreadEventReceiver() {}

	virtual bool event(QEvent *ev) override;
};

}
