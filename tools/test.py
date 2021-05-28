import os
import argparse
from ct_common import execute, get_root_path, rmdir, cmake, build, install


parser = argparse.ArgumentParser()
parser.add_argument("--clean", help="clean before build", action="store_true")
parser.add_argument("--config", help="configuration", default="debug")
parser.add_argument("--ninja", help="build using ninja", action="store_true")
args = parser.parse_args()

source_dir = get_root_path().joinpath("test")
build_dir = get_root_path().joinpath("build/build_test")
install_dir = get_root_path().joinpath(f"build/test/{args.config}")

if args.clean:
    rmdir(build_dir)
    exit()

cmake(source_dir, build_dir, args.ninja)
build(build_dir, args.config)
install(build_dir, install_dir, args.config)

with os.scandir(install_dir.joinpath("bin")) as it:
    for entry in it:
        if entry.name.endswith('.exe') and entry.is_file():
            execute(entry.path)
