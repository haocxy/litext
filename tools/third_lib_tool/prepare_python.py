import util.piputil as pu


REQUIRED_LIB_NAMES = [
    'py7zr',
    'requests',
    'GitPython'
]


def prepare_py_mods():
    pu.update('pip')
    pu.install_libs(REQUIRED_LIB_NAMES)


if __name__ == '__main__':
    prepare_py_mods()
