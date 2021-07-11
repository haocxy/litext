#pragma once

#include <vector>
#include <string>


namespace tool
{


class CmdArgs {
public:
	CmdArgs(const std::vector<std::string> &args)
		: args_(args) {}

	size_t size() const {
		return args_.size();
	}

	const std::string &operator[](size_t i) {
		return args_[i];
	}

	

private:
	std::vector<std::string> args_;
};


}
