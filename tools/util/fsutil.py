import shutil
from pathlib import Path


class Renamer:

    def __init__(self, dir_path: Path):
        self._dir_path = dir_path

    def rename(self, old_file_name: str, new_file_name: str):
        shutil.move(
            src=self._dir_path / old_file_name,
            dst=self._dir_path / new_file_name
        )

