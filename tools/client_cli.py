import os
import subprocess
import argparse
import shutil
import pathlib


def print_colored(str):
    os.system("color")
    green = '\033[92m'
    end = '\033[0m'
    print(f"{green}{str}{end}")


def execute(cmd):
    print_colored(f"executing: {cmd}")
    assert(0 == subprocess.call(cmd))


def get_root_path():
    cur=pathlib.Path(__file__)
    return cur.absolute().parent.parent


parser = argparse.ArgumentParser()
parser.add_argument("--clean", help="clean before build", action="store_true")
args = parser.parse_args()

client_cli_dir = os.path.join(get_root_path(), "client_cli")
build_dir = os.path.join(get_root_path(), "build/client_cli")
output_path = os.path.join(get_root_path(), "build/client_cli/debug/client_cli.exe")

if args.clean:
    print_colored(f"executing: rmdir {build_dir}")
    shutil.rmtree(build_dir) 

execute(f"cmake -S {client_cli_dir} -B {build_dir}")
execute(f"cmake --build {build_dir} --config debug -j {os.cpu_count()}")
execute(output_path)
