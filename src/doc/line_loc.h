#pragma once

#include "row_loc.h"


namespace doc
{


class VLineLoc : public VRowLoc {
public:
	static VLineLoc newLineLocAfterLastRow()
	{
		VLineLoc loc(0, 0);
		loc.setFlag(kAfterLastRow);
		loc.setFlag(kAfterLastLine);
		loc.setFlag(kAfterLastChar);
		return loc;
	}
public:
	VLineLoc() = default;
	VLineLoc(i64 row, i64 line) : VRowLoc(row), m_line(line) {}
    i64 line() const { return m_line; }
	void setLine(i64 line) { m_line = line; }
private:
    i64 m_line = 0;
};


}
