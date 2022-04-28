import os
from pathlib import Path


def cmake_build_and_install(
        source_dir: Path,
        build_dir: Path,
        install_dir: Path,
        build_config: str = 'Release',
        other_params: str = '',
        install_retry_times: int = 1):

    if not build_dir.exists():
        os.makedirs(build_dir)

    generate_cmd: str = f'cmake -S {source_dir} -B {build_dir}' \
                        + f' -DCMAKE_BUILD_TYPE={build_config}'\
                        + f' -DCMAKE_INSTALL_PREFIX={install_dir}'\
                        + f' {other_params}'
    print(f'Executing CMD: {generate_cmd}', flush=True)
    os.system(generate_cmd)

    job_count: int = max(1, os.cpu_count() - 1)

    build_and_install_cmd: str = f'cmake --build {build_dir}'\
                                 + f' -t install --config {build_config}'\
                                 + f' -j {job_count}'
    for i in range(install_retry_times):
        print(f'Executing CMD: {build_and_install_cmd}', flush=True)
        os.system(build_and_install_cmd)
