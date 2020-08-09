import os
import pathlib
import subprocess
import shutil


def execute(cmd):
    print_colored(f"executing: {cmd}")
    assert(0 == subprocess.call(cmd, shell=True))


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


def cmake(source_dir, build_dir):
    execute(f"cmake -S {source_dir} -B {build_dir}")


def build(build_dir, config):
    execute(f"cmake --build {build_dir} --config {config} -j {os.cpu_count()}")
