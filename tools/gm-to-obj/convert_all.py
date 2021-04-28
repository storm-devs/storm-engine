
import os
import ntpath
import sys
import gm_to_obj_converter
import argparse
if (__name__ == "__main__"):
    def _print(*args):
        pass
    gm_to_obj_converter._print = _print
    # define args
    failed = ""

    parser = argparse.ArgumentParser()
    parser.add_argument(
        "path", help="Where are the .gm files located?", type=str)
    args = parser.parse_args()
    resource = str(args.path)

    out = open("FailedFiles.txt", "w")
    for root, dirs, files in os.walk(resource):
        for file in files:
            if file.endswith(".gm"):
                try:
                    args_input = os.path.join(root, file)

                    args_output = ntpath.basename(
                        args_input).replace(".gm", ".obj")

                    args_output = os.path.join(
                        os.path.dirname(args_input), (args_output))
                    print("Input:", args_input)
                    gm_to_obj_converter.convert(args_input, args_output)
                except:
                    out.write(args_input+"\n")
    out.close()
