#include "system_util.h"

#include <cstring>

#if defined(WIN32)
#include <Windows.h>
#else
#include <unistd.h>
#include <sys/sysinfo.h>
#endif


namespace
{

class Initer {
public:
	Initer() {
		init();
	}

	size_t pageSize() const {
		return pageSize_;
	}

	int procCount() const {
		return procCount_;
	}

private:

#if defined(WIN32)
	void init() {
		SYSTEM_INFO systemInfo;
		std::memset(&systemInfo, 0, sizeof(systemInfo));
		GetSystemInfo(&systemInfo);
		pageSize_ = systemInfo.dwPageSize;
		procCount_ = systemInfo.dwNumberOfProcessors;
	}
#else
	void init() {
		pageSize_ = sysconf(_SC_PAGE_SIZE);
		procCount_ = get_nprocs();
	}
#endif

private:
	size_t pageSize_ = 0;
	int procCount_ = 0;
};

static const Initer g_initer;

}


namespace SystemUtil
{

size_t pageSize()
{
	return g_initer.pageSize();
}

int processorCount() {
	return g_initer.procCount();
}

}
