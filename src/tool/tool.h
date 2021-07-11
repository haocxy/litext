#pragma once

#include <sstream>

#include "cmd_args.h"


namespace tool
{


class Tool {
public:
	virtual ~Tool() {}

	void feedCmdArgs(const CmdArgs &args) {
		args_ = args;
		fillArgs();
	}

protected:
	virtual void fillArgs() = 0;

	template <typename T>
	void fillArg(T &obj) {
		if (currentArgIndex_ >= args_.size()) {
			return;
		}
		const std::string &s = args_[currentArgIndex_++];
		std::istringstream ss(s);
		ss >> obj;
	}

private:
	CmdArgs args_;
	size_t currentArgIndex_ = 0;
};


}
