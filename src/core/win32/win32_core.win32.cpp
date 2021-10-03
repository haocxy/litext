#include "win32_core.win32.h"

#include <memory>
#include <sstream>
#include <functional>


namespace win32
{

std::string ErrorCode::message() const {
    char *msgBuffer = nullptr;

    ::FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr, n_, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), (LPSTR)&msgBuffer, 0, nullptr);

    if (!msgBuffer) {
        throw std::runtime_error("win32::ErrorCode::message() failed because ::FormatMessageA() failed");
    }

    std::unique_ptr<void, std::function<void(void *)>> deleter(msgBuffer, [](void *p) {
        ::LocalFree(p);
    });

    std::string msg(msgBuffer);

    while (!msg.empty()) {
        const char ch = msg.back();
        if (ch != '\r' && ch != '\n') {
            break;
        } else {
            msg.pop_back();
        }
    }

    std::ostringstream ss;
    ss << '(' << n_ << ')' << msg;

    return ss.str();
}

}
