import argparse
from ct_common import execute, get_root_path, rmdir, cmake, build, install


parser = argparse.ArgumentParser()
parser.add_argument("--clean", help="clean before build", action="store_true")
parser.add_argument("--config", help="configuration", default="debug")
parser.add_argument("--ninja", help="build using ninja", action="store_true")
args = parser.parse_args()

source_dir = get_root_path().joinpath("client_gui")
build_dir = get_root_path().joinpath("build/build_client_gui")
install_dir = get_root_path().joinpath(f"build/client_gui/{args.config}")

if args.clean:
    rmdir(build_dir)

cmake(source_dir, build_dir, args.ninja)
build(build_dir, args.config)
install(build_dir, install_dir, args.config)
execute(str(install_dir.joinpath("bin/client_gui.exe")))
