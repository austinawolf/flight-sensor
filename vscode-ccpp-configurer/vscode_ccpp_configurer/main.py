import argparse
import os.path
from vscode_ccpp_configurer.config import CCppConfig
from vscode_ccpp_configurer.parsers import Defines, Includes

parser = argparse.ArgumentParser(description='Configure VSCode Propertires File')
parser.add_argument('--path', dest='path',
                    help='')
parser.add_argument('--defines', dest='defines',
                    help='')
parser.add_argument('--define-prefix', dest='define_prefix', required=False,
                     help='')
parser.add_argument('--includes', dest='includes',
                    help='')


def main():
    args = parser.parse_args()

    path = args.path
    defines_raw = args.defines
    define_prefix = args.define_prefix if args.define_prefix else ""
    includes_raw = args.includes

    if path is None:
        raise Exception

    if defines_raw is None and includes_raw is None:
        raise Exception

    file_path = os.path.join(path, ".vscode", "c_cpp_properties.json")
    ccpp_config = CCppConfig.from_json(file_path)

    if defines_raw:
        defines = Defines.from_args(defines_raw, define_prefix)
        ccpp_config.insert_defines(defines.items)

    if includes_raw:
        includes = Includes.from_args(includes_raw)
        ccpp_config.insert_includes(includes.items)

    ccpp_config.to_json()


main()
