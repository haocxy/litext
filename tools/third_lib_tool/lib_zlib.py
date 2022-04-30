from pathlib import Path

from third_lib_tool import logs
from third_lib_tool.util.cmake_util import cmake_build_and_install
from third_lib_tool.util.compress_util import smart_unpack

ZLIB_LIB_NAME = 'zlib-1.2.11'
LIB_VERSION = '1.2.11'


def unpack(archive_file_dir: Path, extract_dir: Path) -> Path:
    return smart_unpack(
        archive_file=archive_file_dir / f'zlib-{LIB_VERSION}.tar.gz',
        unpack_dir=extract_dir,
        base_name='zlib.src'
    )


def install(source_dir: Path, build_dir: Path, install_dir: Path):
    cmake_build_and_install(
        source_dir=source_dir,
        build_dir=build_dir,
        install_dir=install_dir,
        build_config='Release'
    )


def prepare(thirdlibs_repo_dir: Path, base_dir: Path):
    if base_dir.exists():
        logs.log_skip('zlib', base_dir)
        return
    source_dir = unpack(
        archive_file_dir=thirdlibs_repo_dir / 'zlib',
        extract_dir=base_dir
    )
    install(
        source_dir=source_dir,
        build_dir=source_dir / 'build-release',
        install_dir=base_dir / 'install'
    )
