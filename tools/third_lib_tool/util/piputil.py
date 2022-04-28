import subprocess
import sys


def execute_pipcmd(cmd_list):
    subprocess.run([sys.executable, '-m', 'pip'] + cmd_list)


def update(name: str):
    execute_pipcmd(['install', '--upgrade', name])


def install(name: str):
    execute_pipcmd(['install', name])


def uninstall(name: str):
    execute_pipcmd(['uninstall', name])


def install_libs(names):
    for name in names:
        install(name)
