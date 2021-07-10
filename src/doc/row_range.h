#pragma once

#include "doc_define.h"


namespace doc
{

// 行范围
class RowRange {
public:

	// 构造无效对象
	RowRange()
		: rowOffset_(-1)
		, rowCount_(-1) {

	}

	// 使用首行索引和行数构造对象
	// 
	// rowOffset: 首行索引，索引从0开始
	// rowCount: 行数
	// 
	// 例如：
	// RowRange(2, 3) 表示从下标为2的行到下标为4的行，共3行
	// index: 0 1 2 3 4 5 6
	RowRange(RowN rowOffset, RowN rowCount)
		: rowOffset_(rowOffset)
		, rowCount_(rowCount) {

	}

	// 范围是否有效
	// 当且仅当范围起点和行数都不为负数时有效
	operator bool() const {
		return rowOffset_ >= 0 && rowCount_ >= 0;
	}

	// 范围是否空
	bool empty() const {
		return rowCount_ <= 0;
	}

	// 首行偏移行数，即首行前面的行数
	RowN offset() const {
		return rowOffset_;
	}

	// 范围包括的行数
	RowN count() const {
		return rowCount_;
	}

private:
	RowN rowOffset_ = 0;
	RowN rowCount_ = 0;
};


}
