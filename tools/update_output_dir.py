from pathlib import Path
import sys
import shutil


class PreparedDynamicLibTool:

    def __init__(self, prepare_root: Path, output_root: Path):
        self._prepare_root = prepare_root
        self._output_root = output_root

    def copy_dlib(self, dlib_path: str) -> None:
        src_file: Path = self._prepare_root / dlib_path
        dst_file: Path = self._output_root / src_file.name
        print(f"PreparedDynamicLibTool copy_dlib [{src_file}] => [{dst_file}]")
        shutil.copy(src=src_file, dst=dst_file)


def move_dynamic_libs(prepare_root: Path, output_root: Path):
    if sys.platform != 'win32':
        return
    tool = PreparedDynamicLibTool(prepare_root=prepare_root, output_root=output_root)
    tool.copy_dlib('zlib/install/bin/zlib.dll')
    tool.copy_dlib('libpng/install/bin/libpng16.dll')
    tool.copy_dlib('freetype/install/bin/freetype.dll')


def main():
    print(f'Update dynamic libs, args: [{sys.argv}]')
    move_dynamic_libs(
        prepare_root=Path(sys.argv[1]).absolute(),
        output_root=Path(sys.argv[2]).absolute()
    )


if __name__ == '__main__':
    main()
