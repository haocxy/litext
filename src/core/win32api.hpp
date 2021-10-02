#pragma once

#ifndef WIN32
#error "This is only for win32 platform"
#endif

#include <Windows.h>

#include <mutex>
#include <memory>
#include <string>
#include <sstream>
#include <stdexcept>
#include <functional>


namespace win32api
{



class ErrorCode {
public:
	ErrorCode() {}

	ErrorCode(::DWORD n) : n_(n) {}

	static ErrorCode last() {
		return ErrorCode(::GetLastError());
	}

	std::string message() const {
		char *msgBuffer = nullptr;

		::FormatMessageA(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			nullptr, n_, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), (LPSTR) &msgBuffer, 0, nullptr);

		if (!msgBuffer) {
			throw std::runtime_error("win32api::ErrorCode::message() failed because ::FormatMessageA() failed");
		}

		std::unique_ptr<void, std::function<void(void *)>> deleter(msgBuffer, [](void *p) {
			::LocalFree(p);
		});

		return std::string(msgBuffer);
	}

private:
	::DWORD n_ = 0;
};

class Win32LogicError : public std::logic_error {
public:
	Win32LogicError(const std::string &msg) : std::logic_error(msg) {}

	Win32LogicError(const ErrorCode &ec) : std::logic_error(ec.message()) {}
};

class Win32RuntimeError : public std::runtime_error {
public:
	Win32RuntimeError(const std::string &msg) : std::runtime_error(msg) {}

	Win32RuntimeError(const ErrorCode &ec) : std::runtime_error(ec.message()) {}
};

class ObjHandle {
public:
	ObjHandle() {}

	ObjHandle(::HANDLE handle)
		: handle_(handle) {}

	ObjHandle(const ObjHandle &) = delete;

	ObjHandle(ObjHandle &&other) noexcept {
		Lock lockOther(other.mtx_);
		_move(*this, other);
	}

	ObjHandle &operator=(::HANDLE handle) {
		Lock lockThis(mtx_);
		_close();
		handle_ = handle;
		return *this;
	}

	ObjHandle &operator=(const ObjHandle &) = delete;

	ObjHandle &operator=(ObjHandle &&other) noexcept {
		Lock lockThis(mtx_);
		Lock lockOther(other.mtx_);
		_move(*this, other);
		return *this;
	}

	~ObjHandle() {
		_close();
	}

	void close() {
		Lock lock(mtx_);
		_close();
	}

	bool isValid() const {
		Lock lock(mtx_);
		return handle_ != INVALID_HANDLE_VALUE;
	}

	::HANDLE get() const {
		Lock lock(mtx_);
		return handle_;
	}

	::HANDLE take() {
		Lock lock(mtx_);
		::HANDLE handle = handle_;
		handle_ = INVALID_HANDLE_VALUE;
		return handle;
	}

private:
	void _close() {
		if (handle_ != INVALID_HANDLE_VALUE) {
			::CloseHandle(handle_);
			handle_ = INVALID_HANDLE_VALUE;
		}
	}

private:
	static void _move(ObjHandle &to, ObjHandle &from) {
		if (&to != &from) {
			to._close();
			to.handle_ = from.handle_;
			from.handle_ = INVALID_HANDLE_VALUE;
		}
	}

private:
	using Mtx = std::mutex;
	using Lock = std::lock_guard<Mtx>;

	mutable Mtx mtx_;
	::HANDLE handle_ = INVALID_HANDLE_VALUE;
};

class NamedPipeServer {
public:

	enum class DataDirection {
		ToServer, ToClient, Both
	};

	// 管道中的数据以字节为单位还是消息为单位
	// 以字节为单位时需要上层逻辑自己维护消息边界
	// 以消息为单位时操作系统会维护消息边界
	enum class DataUnit { Byte, Message };

	class Info {
	public:
		Info() {}

		void setName(const std::string &name) {
			name_ = name;
		}

		void setOpenMode(DataDirection dir, bool async) {
			dwOpenMode_ = 0;
			switch (dir) {
			case DataDirection::ToServer:
				dwOpenMode_ = PIPE_ACCESS_INBOUND;
				break;
			case DataDirection::ToClient:
				dwOpenMode_ = PIPE_ACCESS_OUTBOUND;
				break;
			case DataDirection::Both:
				dwOpenMode_ = PIPE_ACCESS_DUPLEX;
				break;
			default:
				break;
			}
			if (async) {
				dwOpenMode_ |= FILE_FLAG_OVERLAPPED;
			}
		}

		void setDataUnit(DataUnit unit) {
			switch (unit) {
			case DataUnit::Byte:
				dwPipeMode_ = PIPE_TYPE_BYTE;
				break;
			case DataUnit::Message:
				dwPipeMode_ = PIPE_TYPE_MESSAGE;
				break;
			default:
				break;
			}
		}

		void setOutBufferSize(DWORD size) {
			nOutBufferSize_ = size;
		}

		void setInBufferSize(DWORD size) {
			nInBufferSize_ = size;
		}

		void setDefaultTimeOut(DWORD ms) {
			nDefaultTimeOut_ = ms;
		}

	private:
		std::string name_;
		DWORD dwOpenMode_ = PIPE_ACCESS_DUPLEX;
		DWORD dwPipeMode_ = PIPE_TYPE_BYTE;
		DWORD nMaxInstances_ = PIPE_UNLIMITED_INSTANCES;
		DWORD nOutBufferSize_ = 1024;
		DWORD nInBufferSize_ = 1024;
		DWORD nDefaultTimeOut_ = 0;
		LPSECURITY_ATTRIBUTES lpSecurityAttributes_ = nullptr;

		friend class NamedPipeServer;
	};

	NamedPipeServer() {}

	NamedPipeServer(const Info &info) {
		start(info);
	}

	~NamedPipeServer() {
	}

	void start(const Info &info) {
		handle_ = ::CreateNamedPipeA(
			info.name_.c_str(),
			info.dwOpenMode_,
			info.dwPipeMode_,
			info.nMaxInstances_,
			info.nOutBufferSize_,
			info.nInBufferSize_,
			info.nDefaultTimeOut_,
			info.lpSecurityAttributes_);

		if (!handle_.isValid()) {
			throw Win32LogicError(ErrorCode::last());
		}
	}

	enum class ConnectResult {
		Connected, ClientDisconnected,
	};

	ConnectResult waitConnect() {
		const ::BOOL result = ::ConnectNamedPipe(handle_.get(), nullptr);
		if (result != 0) {
			return ConnectResult::Connected;
		}

		const DWORD ec = ::GetLastError();
		if (ec == ERROR_PIPE_CONNECTED) {
			return ConnectResult::Connected;
		}

		if (ec == ERROR_NO_DATA) {
			return ConnectResult::ClientDisconnected;
		}

		throw Win32LogicError(ErrorCode::last());
	}

	void close() {
		handle_.close();
	}

	::HANDLE getHANDLE() const {
		return handle_.get();
	}

	::HANDLE takeHANDLE() {
		return handle_.take();
	}

private:
	ObjHandle handle_;
};

}
