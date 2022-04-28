import shutil
import sys
import zipfile
from pathlib import Path

NASM_VERSION: str = '2.15.05'


def unpack(archive_file_dir: Path, extract_dir: Path) -> Path:
    archive_file_path: Path = archive_file_dir / f'nasm-{NASM_VERSION}-win64.zip'
    old_extract_file_path: Path = extract_dir / f'nasm-{NASM_VERSION}'
    new_extract_file_path: Path = extract_dir / 'nasm'
    if new_extract_file_path.exists():
        return new_extract_file_path
    with zipfile.ZipFile(archive_file_path) as zf:
        zf.extractall(path=extract_dir)
    shutil.move(src=old_extract_file_path, dst=new_extract_file_path)
    return new_extract_file_path


def main():
    input_file: Path = Path(sys.argv[1])
    print(f'input_file: {input_file}')
    with zipfile.ZipFile(input_file) as zf:
        zf.extractall(path='D:\\tmp')


if __name__ == '__main__':
    main()
