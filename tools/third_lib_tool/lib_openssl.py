import os
import shutil
import sys
from pathlib import Path

from third_lib_tool import logs
from third_lib_tool.util.compress_util import smart_unpack

OPENSSL_VERSION: str = '1.1.1m'


def unpack(archive_file_dir: Path, extract_dir: Path) -> Path:
    return smart_unpack(
        archive_file=archive_file_dir / f'openssl-{OPENSSL_VERSION}.tar.gz',
        unpack_dir=extract_dir,
        base_name='openssl.src'
    )


def get_configure_cmd_line(install_dir: Path) -> str:
    assert(install_dir is not None)
    openssl_dir: Path = install_dir.parent / 'work'
    args_part: str = f'--prefix={install_dir} --openssldir={openssl_dir}'
    if sys.platform == 'win32':
        return f'perl Configure VC-WIN64A {args_part}'
    else:
        return f'./config {args_part}'


def get_make_cmd_name():
    if sys.platform == 'win32':
        return 'nmake'
    else:
        return 'make'


def get_build_cmd_line():
    make_cmd: str = get_make_cmd_name()
    if sys.platform == 'win32':
        return f'{make_cmd}'
    else:
        return f'{make_cmd} -j10'


def get_install_cmd_line():
    make_cmd: str = get_make_cmd_name()
    return f'{make_cmd} install'


def install(src_dir: Path, install_dir: Path, nasm_exe_dir: Path):
    if nasm_exe_dir is not None:
        nasm_exe_path: Path = nasm_exe_dir / 'nasm.exe'
        shutil.copy(src=nasm_exe_path, dst=src_dir)
    old_cwd: Path = Path.cwd()
    os.chdir(src_dir)
    configure_cmd_line: str = get_configure_cmd_line(install_dir=install_dir)
    build_cmd_line: str = get_build_cmd_line()
    install_cmd_line: str = get_install_cmd_line()
    os.system(configure_cmd_line)
    os.system(build_cmd_line)
    os.system(install_cmd_line)
    os.chdir(old_cwd)


def prepare(thirdlibs_repo_dir: Path, base_dir: Path, nasm_exe_dir: Path):
    if base_dir.exists():
        logs.log_skip('openssl', base_dir)
        return
    source_dir: Path = unpack(
        archive_file_dir=thirdlibs_repo_dir / 'openssl',
        extract_dir=base_dir
    )
    install(
        src_dir=source_dir,
        install_dir=base_dir / 'install',
        nasm_exe_dir=nasm_exe_dir
    )


def main():
    unpack(archive_file_dir=Path('D:\\tmp'), extract_dir=Path('D:\\tmp'))


if __name__ == '__main__':
    main()
