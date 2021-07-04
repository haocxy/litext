#pragma once

#include "doc/doc_define.h"
#include "core/flagset.h"


namespace gui
{

class RowLoc {
public:
	static RowLoc newRowLocAfterLastRow()
	{
		RowLoc loc(0);
		loc.setFlag(kAfterLastRow);
		loc.setFlag(kAfterLastLine);
		loc.setFlag(kAfterLastChar);
		return loc;
	}
public:
	RowLoc() :m_flag(kIsNull), m_row(0) {}
	explicit RowLoc(RowN row) : m_row(row) {}
	RowN row() const { return m_row; }
	void setRow(RowN row) { m_row = row; }
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
	RowN m_row = 0;
};

}
