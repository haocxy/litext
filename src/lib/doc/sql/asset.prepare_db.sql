CREATE TABLE IF NOT EXISTS doc
(
	id INTEGER PRIMARY KEY,
	rowoff INTEGER NOT NULL,
	rowcnt INTEGER NOT NULL,
	byteoff INTEGER NOT NULL,
	bytecnt INTEGER NOT NULL
);
