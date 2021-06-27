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
    shutil.rmtree(dir, ignore_errors=True)


def prefix(install_dir):
    return f"-DCMAKE_INSTALL_PREFIX={install_dir}" if install_dir else ""


def cmake(source_dir, build_dir, ninja, install_dir=None):
    execute(
        f"cmake {prefix(install_dir)} -S {source_dir} -B {build_dir} {' -G Ninja' if ninja else ''}")


def build(build_dir, config):
    execute(f"cmake --build {build_dir} --config {config} -j {os.cpu_count()}")


def install(build_dir, config):
    execute(
        f"cmake --install {build_dir} --config {config}")
