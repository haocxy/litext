#include "thread_pool.h"


ThreadPool::ThreadPool(size_t maxThreadCount)
	: maxThreadCount_(maxThreadCount)
{
	for (size_t i = 0; i < maxThreadCount; ++i) {
		std::thread worker([this]() {
			threadLoop();
		});
		threads_.push_back(std::move(worker));
	}
}

ThreadPool::~ThreadPool()
{
	stopped_ = true;

	workQueue_.wakeupConsumer();
	workQueue_.wakeupProducer();

	for (std::thread &th : threads_) {
		try {
			th.join();
		} catch (...) {} // TODO ��ʱ�������е�join�쳣���Ժ󿴿�������ô���������
	}
}

bool ThreadPool::post(std::function<void()> &&work)
{
	return workQueue_.push(std::move(work));
}

void ThreadPool::threadLoop()
{
	while (!stopped_) {
		std::optional<Work> work = workQueue_.pop();
		if (work) {
			(*work)();
		}
	}
}
