from pathlib import Path


def log_skip(lib_name: str, base_dir: Path):
    print(f'Lib [{lib_name}] at dir [{base_dir}] exists and skipped.')
