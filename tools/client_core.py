import argparse
from ct_common import execute, get_root_path, rmdir, cmake, build


parser = argparse.ArgumentParser()
parser.add_argument("--clean", help="clean before build", action="store_true")
parser.add_argument("--config", help="configuration", default="debug")
args = parser.parse_args()

source_dir = get_root_path().joinpath("client_core")
build_dir = get_root_path().joinpath("build/client_core")

if args.clean:
    rmdir(build_dir)

cmake(source_dir, build_dir)
build(build_dir, args.config)
