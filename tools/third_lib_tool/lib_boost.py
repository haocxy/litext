import os
import sys
from pathlib import Path

from third_lib_tool import logs
from third_lib_tool.util.compress_util import smart_unpack

BOOST_LIB_NAME: str = 'boost_1_78_0'

REQUIRED_BOOST_MODULES = [
    'thread',
    'filesystem',
    'iostreams',
    'serialization',
    'program_options'
]


def unpack(unpack_dir: Path, archive_dir: Path) -> Path:
    return smart_unpack(
        archive_file=archive_dir / f'{BOOST_LIB_NAME}.7z',
        unpack_dir=unpack_dir,
        base_name='boost.src'
    )


def get_boost_booststrap_cmd_name() -> str:
    if sys.platform == 'win32':
        return 'bootstrap'
    else:
        return 'sh bootstrap.sh'


def get_boost_b2_cmd_name() -> str:
    if sys.platform == 'win32':
        return 'b2'
    else:
        return './b2'


def make_boost_b2_install_cmd_line(install_dir: Path, modules: list) -> str:
    b2_cmd_name: str = get_boost_b2_cmd_name()
    cmdline: str = ''
    cmdline += b2_cmd_name
    cmdline += f' --prefix=\"{install_dir}\"'
    for module in modules:
        cmdline += f' --with-{module}'
    cmdline += ' variant=release'
    if sys.platform == 'win32':
        cmdline += ' link=static'
        cmdline += ' runtime-link=static'
    cmdline += ' install'
    return cmdline


def install(source_dir: Path, install_dir: Path):
    bootstrap_cmd_name: str = get_boost_booststrap_cmd_name()
    install_cmdline: str = make_boost_b2_install_cmd_line(
        install_dir=install_dir,
        modules=REQUIRED_BOOST_MODULES)
    print(f'Boost b2 cmd: {install_cmdline}')
    old_dir: str = os.getcwd()
    os.chdir(source_dir)
    os.system(f'{bootstrap_cmd_name}')
    os.system(install_cmdline)
    os.chdir(old_dir)


def prepare(thirdlibs_repo_dir: Path, base_dir: Path):
    if base_dir.exists():
        logs.log_skip('boost', base_dir)
        return
    boost_source_dir: Path = unpack(
        unpack_dir=base_dir / 'src.boost',
        archive_dir=thirdlibs_repo_dir / 'boost'
    )
    boost_install_dir = base_dir / 'install'
    install(
        source_dir=boost_source_dir,
        install_dir=boost_install_dir
    )


def main():
    prepare(Path(sys.argv[1]), Path(sys.argv[2]))


if __name__ == '__main__':
    main()
