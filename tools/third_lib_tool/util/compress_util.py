import abc
import os
import shutil
import sys
import tarfile
import multivolumefile
from dataclasses import dataclass
from pathlib import Path
from typing import Optional, IO, BinaryIO, Union, Generator

import py7zr

from third_lib_tool.util.fsutil import move_and_update_dir

SUFFIX_TAR_GZ: str = '.tar.gz'
SUFFIX_TGZ: str = '.tgz'
SUFFIX_7Z: str = '.7z'


def make_tmp_file_name(base: str) -> str:
    return 't-E_m-P-' + base


def check_archive_file_name(name: str, suffix: str):
    if not name.endswith(suffix):
        raise Exception(f'Bad archive file name [{name}], it must be ended with [{suffix}]')


@dataclass
class SmartUnpackInfo:
    archive_file_obj: Union[IO[bytes], BinaryIO]
    archive_file_path: Path
    unpack_dir: Path
    base_name: str

    def base_dir_without_single_root(self, suffix: str) -> Path:
        real_name: str = self.base_name if self.base_name is not None else self.archive_file_path.name[:-len(suffix)]
        return self.unpack_dir / real_name


class SmartArchive:

    @abc.abstractmethod
    def _extract_all_and_get_good_base_dir(self, unpack_dir: Path) -> Path:
        """
        解压全部内容并返回有效的根目录,详见返回值的描述

        :param unpack_dir: 把压缩包的内容解压到的目标目录
        :return: 若压缩包的根只有一个单一目录,则返回这个目录解压后的目录,否则返回解压后其对应的上层目录
        """
        raise Exception('This is interface function')

    def smart_unpack(self, p: SmartUnpackInfo, suffix: str) -> Path:
        tmp_unpack_dir: Path = p.unpack_dir.parent / make_tmp_file_name(p.unpack_dir.name)
        tmp_base_dir: Path = self._extract_all_and_get_good_base_dir(tmp_unpack_dir)
        final_base_name: str = p.base_name if p.base_name is not None else tmp_base_dir.name
        final_base_dir: Path = p.unpack_dir / final_base_name
        if final_base_dir.exists():
            shutil.rmtree(path=final_base_dir, ignore_errors=False)
        os.makedirs(final_base_dir.parent)
        os.rename(src=tmp_base_dir, dst=final_base_dir)
        if tmp_unpack_dir.exists():
            shutil.rmtree(path=tmp_unpack_dir)
        return final_base_dir


class TarGzSmartArchive(SmartArchive):

    def __init__(self, under: tarfile.TarFile):
        self.under = under

    def _extract_all_and_get_good_base_dir(self, unpack_dir: Path) -> Path:
        self.under.extractall(path=unpack_dir)
        children: list[str] = os.listdir(unpack_dir)
        if len(children) == 1:
            return unpack_dir / Path(children[0]).name
        else:
            return unpack_dir


class SevenZipSmartArchive(SmartArchive):

    def __init__(self, under: py7zr.SevenZipFile):
        self.under = under

    def _extract_all_and_get_good_base_dir(self, unpack_dir: Path) -> Path:
        self.under.extractall(path=unpack_dir)
        children: list[str] = os.listdir(unpack_dir)
        if len(children) == 1:
            return unpack_dir / Path(children[0]).name
        else:
            return unpack_dir


def smart_unpack_for_tar_gz(p: SmartUnpackInfo, suffix: str) -> Path:
    check_archive_file_name(p.archive_file_path.name, suffix)
    with tarfile.open(fileobj=p.archive_file_obj) as archive:
        return TarGzSmartArchive(archive).smart_unpack(p, suffix)


def smart_unpack_for_7z(p: SmartUnpackInfo, suffix: str) -> Path:
    check_archive_file_name(p.archive_file_path.name, suffix)
    with py7zr.SevenZipFile(p.archive_file_obj, 'r') as archive:
        return SevenZipSmartArchive(archive).smart_unpack(p, suffix)


def smart_unpack_by_file_obj(p: SmartUnpackInfo) -> Path:

    if p.archive_file_path.name.endswith(SUFFIX_TAR_GZ):
        return smart_unpack_for_tar_gz(p, SUFFIX_TAR_GZ)

    if p.archive_file_path.name.endswith(SUFFIX_TGZ):
        return smart_unpack_for_tar_gz(p, SUFFIX_TGZ)

    if p.archive_file_path.name.endswith(SUFFIX_7Z):
        return smart_unpack_for_7z(p, SUFFIX_7Z)

    raise Exception(f'Unsupported archive file [{p.archive_file_path}]')


def smart_unpack(archive_file: Path, unpack_dir: Path, base_name: str = None) -> Path:
    """
    智能解压,根据压缩包内文件分布情况决定解压后的目录布局,同时能够处理多种压缩格式.
    返回值为解压得到的内容的最上层目录的绝对路径形式,有效内容位于其中

    若 base_name 为 None 且压缩包内的最上层为一个独立的目录,则把这个目录直接做为unpack_dir的子目录.
    例如有一个名为 content.zip 的压缩包位于 src_dir ,其内容的最上层为单一目录 base
    执行调用 smart_unpack('src_dir/content.zip', '/tmp/dest_dir')
    会返回 /tmp/dest_dir/base
    /tmp/dest_dir/base 下的内容为压缩包中 base 目录下的内容

    对于 base_name 为 None 时的其它情况,在 unpack_dir 下新建一个以压缩包去掉后缀的部分为文件名的目录,把压缩包中的内容置于这个目录下.
    例如有一个名为 content.zip 的压缩包位于 src_dir, 其内容的最上层不为单一目录.
    执行调用 smart_unpack('src_dir/content.zip', '/tmp/dest_dir')
    会返回 /tmp/dest_dir/content
    /tmp/dest_dir/content 下的内容为压缩包中最顶层的内容

    若 base_name 不为 None,则以上两种情况都以 /tmp/dest_dir/base_name 为结果.

    目的:
    简化不同风格的压缩包的解压逻辑.有的压缩包会把单一目录做为最上层压缩,而有的压缩包则会直接把很多内容做为最上层压缩.
    这个函数确保解压后的内容都以合适的方式置于单一目录中,以避免和其它已有文件混淆.
    """

    archive_file = archive_file.absolute()
    unpack_dir = unpack_dir.absolute()
    if archive_file.exists():
        with open(archive_file, mode='rb') as archive_file_obj:
            p = SmartUnpackInfo(
                archive_file_obj=archive_file_obj,
                archive_file_path=archive_file,
                unpack_dir=unpack_dir,
                base_name=base_name
            )
            return smart_unpack_by_file_obj(p)
    else:
        with multivolumefile.open(archive_file, mode='rb') as archive_file_obj:
            p = SmartUnpackInfo(
                archive_file_obj=archive_file_obj,
                archive_file_path=archive_file,
                unpack_dir=unpack_dir,
                base_name=base_name
            )
            return smart_unpack_by_file_obj(p)


def main():
    new_base_name: Optional[str] = None
    if len(sys.argv) > 3:
        new_base_name = sys.argv[3]
    base_dir: Path = smart_unpack(
        archive_file=Path(sys.argv[1]),
        unpack_dir=Path(sys.argv[2]),
        base_name=new_base_name
    )
    print(f'base dir: [{base_dir}]')


if __name__ == '__main__':
    main()
