CREATE TABLE IF NOT EXISTS fonts
(
	file_path BLOB NOT NULL,
	face_id INTEGER NOT NULL,
	last_write_time NOT NULL,
	family BLOB NOT NULL,
	is_scalable BOOLEAN NOT NULL,
	is_bold BOOLEAN NOT NULL,
	is_italic BOOLEAN NOT NULL
);
