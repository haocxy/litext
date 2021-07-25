#include "system_util.h"

#include <cstring>
#include <memory>
#include <stdexcept>

#include <boost/process/environment.hpp>

#if defined(WIN32)
#include <Windows.h>
#else
#include <unistd.h>
#include <sys/sysinfo.h>
#include <fontconfig.h>
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

static fs::path userHomeForWindows() {
    const boost::process::native_environment e = boost::this_process::environment();
    const auto driveIt = e.find("HOMEDRIVE");
    if (driveIt == e.end()) {
        throw std::runtime_error("Environment 'HOMEDRIVE' not found");
    }
    const auto pathIt = e.find("HOMEPATH");
    if (pathIt == e.end()) {
        throw std::runtime_error("Environment 'HOMEPATH' not found");
    }
    return fs::absolute(driveIt->to_string() + pathIt->to_string()).lexically_normal();
}

static fs::path userHomeForNotWindows() {
    const boost::process::native_environment e = boost::this_process::environment();
    const auto homeIt = e.find("HOME");
    if (homeIt == e.end()) {
        throw std::runtime_error("Environment 'HOME' not found");
    }
    return fs::absolute(homeIt->to_string()).lexically_normal();
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

fs::path userHome() {
#if defined(WIN32)
    return userHomeForWindows();
#else
    return userHomeForNotWindows();
#endif
}

static void fontsInDirForWindows(const fs::path &dir, std::vector<fs::path> &fonts)
{
    if (!fs::is_directory(dir)) {
        return;
    }

    for (const fs::directory_entry &e : fs::directory_iterator(dir)) {
        if (e.is_regular_file()) {
            fonts.push_back(fs::absolute(e.path()));
        }
    }
}

static std::vector<fs::path> fontsForWindows()
{
    std::vector<fs::path> fonts;

    // 所有用户共用的字体
    fontsInDirForWindows("C:/Windows/Fonts", fonts);

    // 当前用户专用的字体
    fontsInDirForWindows(userHome() / "AppData/Local/Microsoft/Windows/Fonts", fonts);

    return fonts;
}

#ifndef WIN32

static std::vector<fs::path> fontsForUnix()
{
    std::unique_ptr<FcPattern, void(*)(FcPattern *)> pattern(FcPatternCreate(), [](FcPattern *p) {
        FcPatternDestroy(p);
    });

    if (!pattern) {
        return {};
    }

    FcPatternAddBool(pattern.get(), FC_SCALABLE, FcTrue);

    FcConfigSubstitute(nullptr, pattern.get(), FcMatchPattern);
    FcDefaultSubstitute(pattern.get());

    FcResult result = FcResultNoMatch;
    FcFontMatch(nullptr, pattern.get(), &result);
    if (result != FcResultMatch) {
        return {};
    }

    std::unique_ptr<FcFontSet, void(*)(FcFontSet *)> list(FcFontSort(nullptr, pattern.get(), FcTrue, nullptr, &result),
        [](FcFontSet *p) {
        FcFontSetDestroy(p);
    });

    if (!list || result != FcResultMatch) {
        return {};
    }

    std::vector<fs::path> fonts;

    for (int i = 0; i < list->nfont; ++i) {
        FcPattern *font = list->fonts[i];

        FcChar8 *file = nullptr;
        if (FcResultMatch == FcPatternGetString(font, FC_FILE, 0, &file)) {
            f.file = ;
        } else {
            continue;
        }

        fonts.push_back(reinterpret_cast<const char *>(file));
    }

    return fonts;
}

#endif

std::vector<fs::path> fonts()
{
#ifdef WIN32
    return fontsForWindows();
#else
    return fontsForUnix();
#endif
}

}
