#pragma once

#include <string>
#include <cstdint>

#include "core/fs.h"
#include "tool.h"


namespace tool
{


class BigFileMaker : public Tool {
public:
	virtual ~BigFileMaker() {}

protected:
	virtual void fillArgs() override;

private:
	uintmax_t sizeHint_ = 0;
	fs::path path_;
};


}
