#pragma once

#include <mutex>
#include <condition_variable>


class BlockDoor {
public:
	BlockDoor() {}

	void open() {
		std::unique_lock<std::mutex> lock(mtx_);
		opened_ = true;
		cvDoorOpened_.notify_all();
	}

	void waitOpened() {
		std::unique_lock<std::mutex> lock(mtx_);
		while (!opened_) {
			cvDoorOpened_.wait(lock);
		}
	}

private:
	std::condition_variable cvDoorOpened_;
	std::mutex mtx_;
	bool opened_ = false;
};
