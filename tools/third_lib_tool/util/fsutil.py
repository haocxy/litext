import os
import shutil
from pathlib import Path


def mkdirs(path: str):
    if os.path.exists(path):
        return
    else:
        os.makedirs(path)


def move_and_update_dir(src_dir: Path, dst_dir: Path):
    if not src_dir.is_dir():
        raise Exception(f'src dir [{src_dir}] must be dir')
    if dst_dir.exists() and not dst_dir.is_dir():
        raise Exception(f'dst dir [{dst_dir}] must be dir')
    for abs_src_dir, child_dirs, files in os.walk(src_dir, topdown=False):
        relative_dir: Path = Path(abs_src_dir).relative_to(src_dir)
        abs_dst_dir: Path = dst_dir / relative_dir
        if not abs_dst_dir.exists():
            os.makedirs(abs_dst_dir)
        for file in files:
            src_file: Path = Path(abs_src_dir) / file
            dst_file: Path = abs_dst_dir / file
            if dst_file.exists() and dst_file.is_file():
                os.remove(dst_file)
            shutil.move(src=src_file, dst=dst_file)
        if len(os.listdir(abs_src_dir)) == 0:
            os.rmdir(abs_src_dir)
