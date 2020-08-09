import os
import argparse
from ct_common import *


parser = argparse.ArgumentParser()
parser.add_argument("--clean", help="clean before build", action="store_true")
parser.add_argument("--config", help="configuration", default="debug")
args = parser.parse_args()

source_dir = get_root_path().joinpath("test")
build_dir = get_root_path().joinpath("build/test")
output_dir = build_dir.joinpath(args.config)

if args.clean:
    rmdir(build_dir)

cmake(source_dir, build_dir)
build(build_dir, args.config)

with os.scandir(output_dir) as it:
    for entry in it:
        if entry.name.endswith('.exe') and entry.is_file():
            execute(entry.path)
