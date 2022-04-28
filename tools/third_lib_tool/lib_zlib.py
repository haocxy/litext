import shutil
import tarfile
from pathlib import Path


ZLIB_LIB_NAME = 'zlib-1.2.11'


def unpack(archive_file_path: Path, unpack_dir: Path) -> None:
    extract_dir: Path = unpack_dir.parent
    old_source_dir: Path = extract_dir / ZLIB_LIB_NAME
    with tarfile.open(archive_file_path, 'r') as archive:
        archive.extractall(path=extract_dir)
    shutil.move(src=old_source_dir, dst=unpack_dir)
