#pragma once

#include <iostream>
#include <sstream>

#include "cmd_args.h"


namespace tool
{


class Tool {
public:
	Tool() {}

	virtual ~Tool() {}

	int exec(const std::vector<std::string> &args) {
		try {
			args_.setArgs(args);
			init();
			execute();
			return 0;
		}
		catch (const std::exception &e) {
			std::cerr << "Error: " << e.what() << std::endl;
			return 1;
		}
	}

protected:
	virtual void init() = 0;

	virtual void execute() = 0;

	template <typename T>
	void arg(T &obj) {
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
