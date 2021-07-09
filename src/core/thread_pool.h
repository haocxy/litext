#pragma once

#include <atomic>
#include <vector>
#include <thread>
#include <functional>

#include "worker.h"
#include "block_queue.h"


class ThreadPool : public Worker {
public:
	ThreadPool(size_t maxThreadCount);

	~ThreadPool();

	virtual void post(std::function<void()> &&work) override;

private:
	void threadLoop();

private:
	using Work = std::function<void()>;
	std::atomic_bool stopped_ = false;
	const size_t maxThreadCount_;
	std::vector<std::thread> threads_;
	BlockQueue<Work> workQueue_;
};
