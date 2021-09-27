CREATE TABLE IF NOT EXISTS font_files
(
	file_path BLOB PRIMARY KEY,
	last_write_time NOT NULL
);

CREATE TABLE IF NOT EXISTS font_faces
(
	file_path BLOB NOT NULL REFERENCES font_files(file_path),
	face_id INTEGER NOT NULL,
	family BLOB NOT NULL,
	is_scalable BOOLEAN NOT NULL,
	is_bold BOOLEAN NOT NULL,
	is_italic BOOLEAN NOT NULL,
	CONSTRAINT primary_key PRIMARY KEY(file_path, face_id)
)
