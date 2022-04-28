import os.path
import subprocess
import sys
from pathlib import Path

PROTO_FILE_SUFFIX: str = '.proto'
OLD_HEAD_FILE_SUFFIX: str = '.pb.h'
OLD_SOURCE_FILE_SUFFIX: str = '.pb.cc'
MSG_ID_FILE_PREFIX: str = 'gen.pb.'
MSG_ID_FILE_SUFFIX: str = '.h'
MSG_ID_TABLE_FILE_NAME: str = 'msg_id_table.txt'


def load_msg_id_table(msg_table_file: Path) -> dict[str, int]:
    if not msg_table_file.exists():
        return {}
    msg_id_table: dict[str, int] = {}
    with open(msg_table_file, mode='r') as file:
        for line in file:
            parts = line.strip().split(',')
            key: str = parts[0]
            val: str = parts[1]
            msg_id_table[key] = int(val)
    return msg_id_table


def save_msg_id_table(msg_table_file: Path, msg_id_table: dict[str, int]):
    with open(msg_table_file, mode='w') as file:
        for full_type_name, msg_id in msg_id_table.items():
            file.write(str(full_type_name))
            file.write(',')
            file.write(str(msg_id))
            file.write('\n')


def get_protoc_exe_path(project_root: Path) -> Path:
    protoc_dir: Path = project_root / 'prepare' / 'protobuf' / 'install' / 'bin'
    if sys.platform == 'win32':
        return protoc_dir / 'protoc.exe'
    else:
        return protoc_dir / 'protoc'


def should_compile_proto_file(proto_file: Path, generated_file: Path) -> bool:
    if not proto_file.exists():
        return False
    proto_file_mtime = os.path.getmtime(proto_file)
    if not generated_file.exists():
        return True
    generated_file_mtime = os.path.getmtime(generated_file)
    return proto_file_mtime >= generated_file_mtime


class ProtoFile:

    def __init__(self, proto_file: Path):
        self._proto_file = proto_file

    def should_compile(self, generated_file: Path) -> bool:
        return should_compile_proto_file(
            proto_file=self._proto_file,
            generated_file=generated_file
        )


def compile_one_proto_file(protoc_exe: Path, proto_file: Path):
    proto_file_name: str = proto_file.name
    if not proto_file_name.endswith(PROTO_FILE_SUFFIX):
        return
    base_name: str = proto_file_name[0: -len(PROTO_FILE_SUFFIX)]
    old_head_file_name: str = base_name + OLD_HEAD_FILE_SUFFIX
    old_source_file_name: str = base_name + OLD_SOURCE_FILE_SUFFIX
    dir_part: Path = proto_file.parent
    old_head_file_path: Path = dir_part / old_head_file_name
    old_source_file_path: Path = dir_part / old_source_file_name
    pf: ProtoFile = ProtoFile(proto_file)
    if not pf.should_compile(old_head_file_path) and not pf.should_compile(old_source_file_path):
        return
    print(f'Compile .proto file [{proto_file}]')
    res: subprocess.CompletedProcess = subprocess.run(
        args=[protoc_exe, '--cpp_out=.', proto_file_name],
        cwd=proto_file.parent,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT
    )
    if res.returncode != 0:
        raise Exception(f'Compile proto file [{proto_file}] failed because [{res.stdout.strip()}]')


def each_proto_file(src_root: Path):
    proto_files: list[Path] = []
    for proto_file in src_root.rglob(pattern='*' + PROTO_FILE_SUFFIX):
        proto_files.append(proto_file)
    # 对全部找到的协议文件排序,确保最终结果的稳定
    proto_files.sort()
    for proto_file in proto_files:
        yield proto_file


def compile_proto_files(protoc_exe: Path, src_root: Path):
    for proto_file in each_proto_file(src_root):
        compile_one_proto_file(
            protoc_exe=protoc_exe,
            proto_file=proto_file
        )


class IdGenerator:

    def __init__(self, init_value: int):
        self._id = init_value

    @property
    def next(self) -> int:
        n = self._id
        self._id = n + 1
        return int(n)


class FileWriter:

    def __init__(self, file):
        self._file = file

    def line(self, text=None):
        if text is not None:
            self._file.write(text)
        self._file.write('\n')


def convert_package_line_to_namespace(line: str) -> str:
    package_part: str = line.split(';')[0].split(' ')[1]
    return package_part.replace('.', '::')


def get_type_name_from_message_line(line: str) -> str:
    part1: str = line.split(' ')[1]
    type_name: str = ''
    for c in part1:
        if c.isalnum():
            type_name += c
    return type_name


def make_full_type_name(type_name: str, namespace_name: str):
    if namespace_name is None:
        return type_name
    else:
        return namespace_name + '::' + type_name


def get_msg_id(full_type_name: str, id_generator: IdGenerator, msg_id_table: dict[str, int]) -> int:
    if full_type_name in msg_id_table:
        return msg_id_table[full_type_name]
    else:
        next_id: int = id_generator.next
        msg_id_table[full_type_name] = next_id
        return next_id


def handle_proto_line_message(id_generator: IdGenerator, line: str, namespace_name: str, msg_id_table: dict[str, int]) -> str:
    type_name: str = get_type_name_from_message_line(line)
    full_name: str = make_full_type_name(type_name=type_name, namespace_name=namespace_name)
    msg_id: int = get_msg_id(
        full_type_name=full_name,
        id_generator=id_generator,
        msg_id_table=msg_id_table
    )
    sep: str = '\n'
    result: str = ''
    result += 'template <>'
    result += sep
    result += f'struct MsgTypeTraits<{full_name}> ' + '{'
    result += sep
    result += '    static const core::net::MsgTypeId Value{ %s };' % msg_id
    result += sep
    result += '};'
    result += sep
    return result


def should_update_msg_id_file(proto_file: Path, msg_id_file: Path):
    return True
    if not proto_file.exists():
        return False
    if not msg_id_file.exists():
        return True
    return os.path.getmtime(proto_file) >= os.path.getmtime(msg_id_file)


def generate_one_msg_id_file(id_generator: IdGenerator, proto_file: Path, msg_id_table: dict[str, int]):
    if not proto_file.name.endswith(PROTO_FILE_SUFFIX):
        return
    base_name: str = proto_file.name[0:-len(PROTO_FILE_SUFFIX)]
    generated_file_name: str = f'{MSG_ID_FILE_PREFIX}{base_name}{MSG_ID_FILE_SUFFIX}'
    generated_file_path: Path = proto_file.parent / generated_file_name
    namespace_found: bool = False
    namespace_name = None
    if not should_update_msg_id_file(proto_file=proto_file, msg_id_file=generated_file_path):
        return
    print(f'Generate msg id file for [{proto_file}]')
    with open(generated_file_path, mode='w') as out_file:
        with open(proto_file, mode='r') as in_file:
            w: FileWriter = FileWriter(out_file)
            w.line(f'#include <stdint.h>')
            w.line(f'#include "core/msg/msg_type_traits.h"')
            w.line(f'#include "{base_name}{OLD_HEAD_FILE_SUFFIX}"')
            w.line()
            for raw_line in in_file:
                line: str = raw_line.strip()
                if line.startswith('package ') and not namespace_found:
                    namespace_found = True
                    namespace_name = convert_package_line_to_namespace(line)
                if line.startswith('message '):
                    s = handle_proto_line_message(
                        id_generator=id_generator,
                        line=line,
                        namespace_name=namespace_name,
                        msg_id_table=msg_id_table
                    )
                    w.line(s)
        w.line()


def get_all_type_names_in_one_proto_file(proto_file: Path, all_type_names: set[str]):
    if not proto_file.exists():
        return
    namespace_found: bool = False
    namespace_name = None
    with open(proto_file, mode='r') as file:
        for raw_line in file:
            line: str = raw_line.strip()
            if line.startswith('package ') and not namespace_found:
                namespace_found = True
                namespace_name = convert_package_line_to_namespace(line)
            if line.startswith('message '):
                type_name: str = get_type_name_from_message_line(line)
                full_name: str = make_full_type_name(type_name=type_name, namespace_name=namespace_name)
                all_type_names.add(full_name)


def get_max_id_in_msg_id_table(msg_id_table: dict[str, int]) -> int:
    max_id: int = 0
    for key, val in msg_id_table.items():
        if val > max_id:
            max_id = val
    return max_id


def remove_unused_table_items(msg_id_table: dict[str, int], all_type_names: set[str]):
    to_remove: set[str] = set()
    for msg_name in msg_id_table.keys():
        if msg_name not in all_type_names:
            to_remove.add(msg_name)
    for msg_name in to_remove:
        del msg_id_table[msg_name]


def generate_msg_id_files(src_root: Path):
    msg_table_file_path: Path = src_root / 'proto' / MSG_ID_TABLE_FILE_NAME
    msg_id_table: dict[str, int] = load_msg_id_table(msg_table_file=msg_table_file_path)
    max_id: int = get_max_id_in_msg_id_table(msg_id_table=msg_id_table)
    id_generator: IdGenerator = IdGenerator(init_value=max_id + 1)
    all_type_names: set[str] = set()
    for proto_file in each_proto_file(src_root):
        generate_one_msg_id_file(
            id_generator=id_generator,
            proto_file=proto_file,
            msg_id_table=msg_id_table
        )
        get_all_type_names_in_one_proto_file(
            proto_file=proto_file,
            all_type_names=all_type_names
        )
    remove_unused_table_items(
        msg_id_table=msg_id_table,
        all_type_names=all_type_names
    )
    save_msg_id_table(
        msg_table_file=msg_table_file_path,
        msg_id_table=msg_id_table
    )


def delete_unused_files(project_root: Path):
    # TODO
    pass


def main():
    project_root: Path = Path(sys.argv[1])
    delete_unused_files(project_root)
    protoc_exe: Path = get_protoc_exe_path(project_root)
    src_root: Path = project_root / 'src'
    compile_proto_files(
        protoc_exe=protoc_exe,
        src_root=src_root
    )
    # 注意处理顺序
    # 必须先生成由protoc生成的文件,再生成消息ID文件
    generate_msg_id_files(
        src_root=src_root
    )


if __name__ == '__main__':
    main()
