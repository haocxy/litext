from pathlib import Path

import git


def download(clone_dir: Path):
    if clone_dir.exists():
        print(f'Pulling repo at [{clone_dir}]')
        repo = git.Repo(clone_dir)
        repo.remote('origin').pull()
    else:
        print(f'Cloning repo to [{clone_dir}]')
        git.Repo.clone_from(
            url='https://gitee.com/haocxy/thirdlibs.git',
            to_path=clone_dir,
            progress=lambda op_code, cur_count, max_count, message:
                print(f'Clone git repo (thirdlibs) [{cur_count / max_count * 100}%]:{message}')
        )
