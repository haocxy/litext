#include "system_util.h"

#include <cstring>

#if __has_include(<Windows.h>)
#include <Windows.h>
#elif __has_include(<sys/sysinfo.h>)
#include <sys/sysinfo.h>
#else
#error "unsupported platform"
#endif


namespace
{

class ProcCountIniter {
public:
	ProcCountIniter()
		: procCount_(getProcCountBySystemAPI()) {}

#if __has_include(<Windows.h>)
	static int getProcCountBySystemAPI() {
		SYSTEM_INFO systemInfo;
		std::memset(&systemInfo, 0, sizeof(systemInfo));
		::GetSystemInfo(&systemInfo);
		return systemInfo.dwNumberOfProcessors;
	}
#elif __has_include(<sys/sysinfo.h>)
	static int getProcCountBySystemAPI() {
		return ::get_nprocs();
	}
#else
	#error "unsupported platform"
#endif

	const int procCount_;
};

static ProcCountIniter g_procCountIniter;

}


namespace SystemUtil
{

int processorCount() {
	return g_procCountIniter.procCount_;
}

}
