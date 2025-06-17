#!/usr/bin/env python3

from os import chmod, path, scandir
import shutil
import subprocess
import sys

pwd = path.dirname(path.realpath(__file__))
built = sys.argv[1]
mdepl = sys.argv[2]


def get_deps(fn):
    """Returns dependencies of a binary with `otool -L ...`"""
    res = []
    out = subprocess.check_output(f'otool -L {fn}', shell=True, cwd=pwd, text=True)
    for line in out.split(':')[1].split('\n'):
        if not line:
            continue
        dep = line.strip().split(' ')[0]
        if path.basename(dep) != path.basename(fn) and not dep.startswith('/usr') and not dep.startswith('/System') and '.framework' not in dep:
            res.append(dep)
    return res


if __name__ == '__main__':
    # Creates list of dependencies obtained by macdeployqt
    macdeploy = []
    with scandir(path.join(mdepl, 'Contents/Frameworks')) as entries:
        for entry in entries:
            if entry.is_file():
                macdeploy.append(entry.name)

    # Creates list of dependencies obtained by recursively calling `otool -L`
    res = get_deps(path.join(built, 'Contents/MacOS/OpenHKL'))
    N = len(res)
    while True:
        for dep in res:
            if not dep.startswith('@loader_path'):
                more = get_deps(dep)
            if more:
                for subdep in more:
                    if subdep.startswith('@rpath'):
                        subdep = path.join(path.dirname(dep), path.basename(subdep))
                    if subdep not in res:
                        res.append(subdep)
        if len(res) == N:
            break
        N = len(res)
        # print(N)

    # subtract list created by macdeploy from the one recursively obtained
    missing = set([path.basename(dep) for dep in res]) - set(macdeploy)

    print('Copying missing libs')
    for dep in sorted(res):
        if path.basename(dep) in missing:
            print(dep, path.realpath(dep))
            shutil.copy(path.realpath(dep), path.join(mdepl, 'Contents/Frameworks', path.basename(dep)))
