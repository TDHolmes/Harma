#!/usr/bin/env python
# -*- coding: UTF-8 -*-
import os

import pensel_utils


def build_documentation(verbose=False):
    stdout, stderr, retcode = pensel_utils.run_command("doxygen", verbose)
    os.chdir("output/latex/")
    stdout, stderr, retcode = pensel_utils.run_command("make", verbose)
    os.chdir("../../")


if __name__ == '__main__':
    import argparse

    parser = argparse.ArgumentParser()
    parser.add_argument("-v", "--verbose", action="store_true",
                        help="Shows the individual calls made by make.")

    args = parser.parse_args()

    os.chdir("../documentation/Firmware Documentation")
    # make sure all our folder paths exist
    current_path = os.path.abspath("./")

    build_documentation(args.verbose)
