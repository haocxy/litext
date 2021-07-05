#pragma once

#include "view_define.h"
#include "row_loc.h"


namespace gui
{


class LineLoc : public VRowLoc {
public:
	static LineLoc newLineLocAfterLastRow()
	{
		LineLoc loc(0, 0);
		loc.setFlag(kAfterLastRow);
		loc.setFlag(kAfterLastLine);
		loc.setFlag(kAfterLastChar);
		return loc;
	}
public:
	LineLoc() = default;
	LineLoc(RowN row, LineN line) : VRowLoc(row), m_line(line) {}
	LineN line() const { return m_line; }
	void setLine(LineN line) { m_line = line; }
private:
	LineN m_line = 0;
};


}
