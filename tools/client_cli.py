import argparse
from ct_common import execute, get_root_path, rmdir, cmake, build


parser = argparse.ArgumentParser()
parser.add_argument("--clean", help="clean before build", action="store_true")
parser.add_argument("--config", help="configuration", default="debug")
parser.add_argument("--ninja", help="build using ninja", action="store_true")
args = parser.parse_args()

source_dir = get_root_path().joinpath("client_cli")
build_dir = get_root_path().joinpath("build/client_cli")
output_path = build_dir.joinpath(f"{args.config}/client_cli")

if args.clean:
    rmdir(build_dir)

cmake(source_dir, build_dir, args.ninja)
build(build_dir, args.config)
execute(str(output_path))
