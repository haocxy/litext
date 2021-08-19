#pragma once

#include "core/flagset.h"
#include "core/basetype.h"


namespace doc
{

class VRowLoc {
public:
	static VRowLoc newRowLocAfterLastRow()
	{
		VRowLoc loc(0);
		loc.setFlag(kAfterLastRow);
		loc.setFlag(kAfterLastLine);
		loc.setFlag(kAfterLastChar);
		return loc;
	}
public:
	VRowLoc() :m_flag(kIsNull), row_(0) {}
	explicit VRowLoc(i64 row) : row_(row) {}
    i64 row() const { return row_; }
	void setRow(i64 row) { row_ = row; }
	bool isNull() const { return hasFlag(kIsNull); }
	bool isAfterLastRow() const { return hasFlag(kAfterLastRow); }
protected:
	enum {
		kIsNull,
		kAfterLastRow,
		kAfterLastLine,
		kAfterLastChar,
		kFlagCnt,
	};
	bool hasFlag(int f) const { return m_flag.has(f); }
	void clearFlag(int f) { m_flag.unset(f); }
	void setFlag(int f) { m_flag.set(f); }
private:
	FlagSet<kFlagCnt> m_flag;
    i64 row_ = 0;
};

}
