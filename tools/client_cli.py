import os
import argparse
from ct_common import *


parser = argparse.ArgumentParser()
parser.add_argument("--clean", help="clean before build", action="store_true")
args = parser.parse_args()

source_dir = get_root_path().joinpath("client_cli")
build_dir = get_root_path().joinpath("build/client_cli")
output_path = build_dir.joinpath("Debug/client_cli")

if args.clean:
    rmdir(build_dir)

cmake(source_dir, build_dir)
build(build_dir, "debug")
execute(str(output_path))
