#pragma once

#include "charset.h"


namespace doc
{

class DocumentListener {
public:
	virtual ~DocumentListener() {}

	virtual void onDocumentPartLoaded() = 0;
};

}
