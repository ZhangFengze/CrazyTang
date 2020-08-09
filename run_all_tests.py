import os
import subprocess


def print_colored(str):
    os.system("color")
    green = '\033[92m'
    end = '\033[0m'
    print(f"{green}{str}{end}")


def execute(cmd):
    print_colored("executing: {0}".format(cmd))
    assert(0 == subprocess.call(cmd))


execute("cmake -S test -B build/test")
execute(
    "cmake --build build/test --config debug -j {0}".format(os.cpu_count()))

dir = "./build/test/Debug"
with os.scandir(dir) as it:
    for entry in it:
        if entry.name.endswith('.exe') and entry.is_file():
            execute(entry.path)
