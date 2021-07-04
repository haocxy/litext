#pragma once

#include "line_loc.h"


namespace gui
{


class CharLoc : public LineLoc {
public:
	static CharLoc newCharLocAfterLastRow()
	{
		CharLoc loc(0, 0, 0);
		loc.setFlag(kAfterLastRow);
		loc.setFlag(kAfterLastLine);
		loc.setFlag(kAfterLastChar);
		return loc;
	}
	static CharLoc newCharLocAfterLastChar(const LineLoc &lineLoc)
	{
		CharLoc loc(lineLoc, 0);
		loc.setFlag(kAfterLastChar);
		return loc;
	}
public:
	CharLoc() = default;
	CharLoc(const LineLoc &lineLoc, CharN col) : LineLoc(lineLoc), m_col(col) {}
	CharLoc(int row, int line, CharN col) :LineLoc(row, line), m_col(col) {}
	CharN col() const { return m_col; }
	void setCol(CharN col) { m_col = col; }
	bool isAfterLastChar() const { return hasFlag(kAfterLastChar); }
private:
	CharN m_col = 0;
};


}
