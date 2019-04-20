import argparse
import os
import sys
sys.path.append(str(os.path.dirname(os.path.abspath(__file__)))+'/../../../../libs')
sys.path.append(str(os.path.dirname(os.path.abspath(__file__)))+'/../../../../libs/commonapi_tools')

from commonapi_tools.fidl_parser import parse_interfaces

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument("capi_interface",
                        help="CommonAPI interface /<path>/<name>.fidl")
    args = parser.parse_args()
    try:
        sys.stdout = open(os.devnull, 'w')
        interfaces = parse_interfaces(args.capi_interface)
        sys.stdout = sys.__stdout__
        for interface in interfaces:
            package_name = interface.package_name.replace(".", "\/")
            print("v{0}/{1}/{2};".format(interface.major, package_name, interface.name))
    except Exception as ex:
        print("ex is " + str(ex))
        exit(1)
