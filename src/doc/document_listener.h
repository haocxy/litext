#pragma once

#include "charset.h"
#include "doc_define.h"


namespace doc
{

// 文档监听器，单线程使用，非线程安全
class DocumentListener {
public:
	virtual ~DocumentListener() {}

	// 文档的一部分加载完成
	virtual void onDocumentPartLoaded(Charset charset, RowN offset, RowN rowCount) = 0;

	// 整个文档加载完成
	virtual void onDocumentLoaded() = 0;

	// 文档被其它进程修改
	virtual void onDocumentModifiedByOtherProc() = 0;

	// 文档被其它进程删除
	virtual void onDocumentRemovedByOtherProc() = 0;
};

}
