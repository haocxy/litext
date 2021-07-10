#pragma once

#include "core/worker.h"
#include "run_in_gui_thread_event_receiver.h"


class QObject;


namespace gui::qt
{


class GuiThreadWorker : public Worker {
public:
	GuiThreadWorker() {}

	virtual ~GuiThreadWorker() {}

	virtual void post(std::function<void()> &&action) override;

private:
	RunInGuiThreadEventReceiver receiver_;
};


}
