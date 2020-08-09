import os
import subprocess


def print_colored(str):
    os.system("color")
    green = '\033[92m'
    end = '\033[0m'
    print(f"{green}{str}{end}")


def execute(cmd):
    print_colored(f"executing: {cmd}")
    assert(0 == subprocess.call(cmd))


def get_root_path():
    return os.path.dirname(os.path.abspath(__file__))


test_dir = os.path.join(get_root_path(), "test")
build_dir = os.path.join(get_root_path(), "build/test")

execute(f"cmake -S {test_dir} -B {build_dir}")
execute(f"cmake --build {build_dir} --config debug -j {os.cpu_count()}")

with os.scandir(os.path.join(build_dir, "Debug")) as it:
    for entry in it:
        if entry.name.endswith('.exe') and entry.is_file():
            execute(entry.path)
