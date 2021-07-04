#pragma once

#include "doc/doc_define.h"
#include "view_define.h"
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

class LineLoc : public RowLoc {
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
	LineLoc(RowN row, LineN line) :RowLoc(row), m_line(line) {}
	LineN line() const { return m_line; }
	void setLine(LineN line) { m_line = line; }
private:
	LineN m_line = 0;
};

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
