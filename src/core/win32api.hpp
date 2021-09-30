#pragma once

#ifndef WIN32
#error "This is only for win32 platform"
#endif

#include <Windows.h>

#include <string>
#include <sstream>
#include <memory>
#include <stdexcept>


namespace win32api
{

class Win32LogicError : public std::logic_error {
public:
	Win32LogicError(const std::string &msg) : std::logic_error(msg) {}
};

class Win32RuntimeError : public std::runtime_error {
public:
	Win32RuntimeError(const std::string &msg) : std::runtime_error(msg) {}
};

inline std::string lastErrMsg()
{
	std::ostringstream ss;
	const DWORD code = ::GetLastError();
	ss << "Windows API Error: [" << code << "]";
	return ss.str();
}

class ObjHandle {
public:
	ObjHandle() {}

	ObjHandle(::HANDLE handle)
		: handle_(handle) {}

	ObjHandle(const ObjHandle &) = delete;

	ObjHandle(ObjHandle &&other) noexcept {
		move(*this, other);
	}

	ObjHandle &operator=(::HANDLE handle) {
		close();
		handle_ = handle;
		return *this;
	}

	ObjHandle &operator=(const ObjHandle &) = delete;

	ObjHandle &operator=(ObjHandle &&other) noexcept {
		move(*this, other);
		return *this;
	}

	~ObjHandle() {
		close();
	}

	void close() {
		if (handle_ != INVALID_HANDLE_VALUE) {
			::CloseHandle(handle_);
			handle_ = INVALID_HANDLE_VALUE;
		}
	}

	bool isValid() const {
		return handle_ != INVALID_HANDLE_VALUE;
	}

	::HANDLE get() const {
		return handle_;
	}

	::HANDLE take() {
		::HANDLE handle = handle_;
		handle_ = INVALID_HANDLE_VALUE;
		return handle;
	}

private:
	static void move(ObjHandle &to, ObjHandle &from) {
		if (&to != &from) {
			to.close();
			to.handle_ = from.handle_;
			from.handle_ = INVALID_HANDLE_VALUE;
		}
	}

private:
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
			throw Win32LogicError(lastErrMsg());
		}
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
