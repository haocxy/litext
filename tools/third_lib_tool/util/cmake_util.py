import os
from pathlib import Path
from typing import Optional


def empty_str(s: Optional[str]) -> bool:
    return s is None or len(s) == 0


def mk_cmake_prefix_path_str(cmake_prefix_path: Optional[list[str]]) -> str:
    if cmake_prefix_path is None or len(cmake_prefix_path) == 0:
        return ''
    result: str = ''
    is_first: bool = True
    for path in cmake_prefix_path:
        if not is_first:
            result += ':'
        result += path
        is_first = False
    return result


def cmake_build_and_install(
        source_dir: Path,
        build_dir: Path,
        install_dir: Path,
        build_config: str = 'Release',
        other_params: str = '',
        install_retry_times: int = 1,
        cmake_prefix_path: Optional[list[str]] = None):

    if not build_dir.exists():
        os.makedirs(build_dir)

    generate_cmd: str = ''
    generate_cmd += f'cmake -S {source_dir} -B {build_dir}'
    generate_cmd += f' -DCMAKE_BUILD_TYPE={build_config}'
    generate_cmd += f' -DCMAKE_INSTALL_PREFIX={install_dir}'

    cmake_prefix_path_str: str = mk_cmake_prefix_path_str(cmake_prefix_path)
    if not empty_str(cmake_prefix_path_str):
        generate_cmd += f' -DCMAKE_PREFIX_PATH={cmake_prefix_path_str}'

    if not empty_str(other_params):
        generate_cmd += f' {other_params}'


    print(f'Executing CMD: {generate_cmd}', flush=True)
    os.system(generate_cmd)

    job_count: int = max(1, os.cpu_count() - 1)

    build_and_install_cmd: str = f'cmake --build {build_dir}'\
                                 + f' -t install --config {build_config}'\
                                 + f' -j {job_count}'
    for i in range(install_retry_times):
        print(f'Executing CMD: {build_and_install_cmd}', flush=True)
        os.system(build_and_install_cmd)
