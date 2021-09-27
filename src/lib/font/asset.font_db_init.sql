CREATE TABLE IF NOT EXISTS fonts
(
	file_path BLOB PRIMARY KEY,
	last_write_time INTEGER NOT NULL,
	family BLOB NOT NULL,
	is_scalable BOOLEAN NOT NULL,
	is_bold BOOLEAN NOT NULL,
	is_italic BOOLEAN NOT NULL
);
