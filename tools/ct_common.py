import os
import pathlib
import subprocess
import shutil


def execute(cmd):
    print_colored(f"executing: {cmd}")
    assert(0 == subprocess.call(cmd))


def get_root_path():
    cur = pathlib.Path(__file__)
    return cur.absolute().parent.parent


def print_colored(str):
    os.system("color")
    green = '\033[92m'
    end = '\033[0m'
    print(f"{green}{str}{end}")


def rmdir(dir):
    print_colored(f"executing: rmdir {dir}")
    shutil.rmtree(dir)
