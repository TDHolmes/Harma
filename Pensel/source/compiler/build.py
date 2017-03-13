#!/usr/bin/env python
# -*- coding: UTF-8 -*-
import sys
import os
import subprocess
from threading import Thread
from Queue import Queue, Empty
import time


class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'
    COLOR_NC = '\033[0m'  # No Color
    COLOR_WHITE = '\033[1;37m'
    COLOR_BLACK = '\033[0;30m'
    COLOR_BLUE = '\033[0;34m'
    COLOR_LIGHT_BLUE = '\033[1;34m'
    COLOR_GREEN = '\033[0;32m'
    COLOR_LIGHT_GREEN = '\033[1;32m'
    COLOR_CYAN = '\033[0;36m'
    COLOR_LIGHT_CYAN = '\033[1;36m'
    COLOR_RED = '\033[0;31m'
    COLOR_LIGHT_RED = '\033[1;31m'
    COLOR_PURPLE = '\033[0;35m'
    COLOR_LIGHT_PURPLE = '\033[1;35m'
    COLOR_BROWN = '\033[0;33m'
    COLOR_YELLOW = '\033[1;33m'
    COLOR_GRAY = '\033[0;30m'
    COLOR_LIGHT_GRAY = '\033[0;37m'


def build_debug(use_clang=False, armgcc_dir="", verbose=False):
    """Builds a debug version of the source code."""
    sys.stdout.write(bcolors.COLOR_YELLOW)
    print("\n————————————————————— DEBUG BUILD ————————————————————————\n")
    sys.stdout.write(bcolors.COLOR_NC)
    sys.stdout.flush()

    # build the makefile with cmake
    if armgcc_dir == "":
        cmd = 'cmake -DCMAKE_TOOLCHAIN_FILE="{}.cmake" -G ' + \
            '"Unix Makefiles" -Wno-deprecated --no-warn-unused-cli ' + \
            '-DCMAKE_BUILD_TYPE=Debug  .'
    else:
        cmd = 'cmake -DCMAKE_TOOLCHAIN_FILE="{}.cmake" -G ' + \
            '"Unix Makefiles" -Wno-deprecated --no-warn-unused-cli ' + \
            '-DCMAKE_BUILD_TYPE=Debug -DTOOLCHAIN_DIR:STRING="' + \
            armgcc_dir + '"  .'

    if use_clang:
        cmd = cmd.format("clang")
    else:
        cmd = cmd.format("armgcc")

    print("executing {}".format(cmd))
    os.system(cmd)

    # make the code
    cmd = "make -j4"
    if verbose:
        cmd += " VERBOSE=1"

    retval = os.system(cmd)
    if retval:
        sys.stdout.write(bcolors.COLOR_LIGHT_RED)
        print("\n\Debug build FAILED!\n")
        sys.stdout.write(bcolors.COLOR_NC)
        sys.stdout.flush()
    return retval


def build_release(use_clang=False, armgcc_dir="", verbose=False):
    """Builds a release version of the source code."""
    sys.stdout.write(bcolors.COLOR_YELLOW)
    print("\n————————————————————— RELEASE BUILD ————————————————————————\n")
    sys.stdout.write(bcolors.COLOR_NC)
    sys.stdout.flush()

    # build the makefile with cmake
    if armgcc_dir == "":
        cmd = 'cmake -DCMAKE_TOOLCHAIN_FILE="{}.cmake" -G ' + \
            '"Unix Makefiles" -Wno-deprecated --no-warn-unused-cli ' + \
            '-DCMAKE_BUILD_TYPE=Release  .'
    else:
        cmd = 'cmake -DCMAKE_TOOLCHAIN_FILE="{}.cmake" -G ' + \
            '"Unix Makefiles" -Wno-deprecated --no-warn-unused-cli ' + \
            '-DCMAKE_BUILD_TYPE=Release -DTOOLCHAIN_DIR:STRING="' + \
            armgcc_dir + '"  .'

    if use_clang:
        cmd = cmd.format("clang")
    else:
        cmd = cmd.format("armgcc")

    print("executing {}".format(cmd))
    os.system(cmd)

    # make the code
    cmd = "make -j4"
    if verbose:
        cmd += " VERBOSE=1"

    retval = os.system(cmd)
    if retval:
        sys.stdout.write(bcolors.COLOR_LIGHT_RED)
        print("\n\tRelease build FAILED!\n")
        sys.stdout.write(bcolors.COLOR_NC)
        sys.stdout.flush()
    return retval


def build_clean(remove_elf_files=True):
    """Removes build files and optionally, the output elf files."""
    if remove_elf_files:
        cmd = "rm -rf debug release"
        run_command(cmd)
    cmd = "rm -rf Makefile cmake_install.cmake CMakeCache.txt"
    run_command(cmd)
    cmd = "rm -rf CMakeCache.txt CMakeFiles/"
    run_command(cmd)


def build_display_size(build_type, chip_total_flash):
    """
    Expected output from the arm toolchain size function:
    text    data     bss     dec     hex filename
    6772     112     664    7548    1d7c debug/motor_driver.elf

    Adds in a percentage measurement.
    """
    chip_total_flash = chip_total_flash * 1024
    sys.stdout.write(bcolors.COLOR_YELLOW)
    b = build_type.upper()
    print("\n———————————— {} BUILD SIZE ———————————————\n".format(b))
    sys.stdout.write(bcolors.COLOR_NC)
    sys.stdout.flush()
    file_location = "{}/motor_driver.elf".format(build_type)
    cmd = "arm-none-eabi-size {}".format(file_location)
    stdout, stderr, retval = run_command(cmd, print_output=False)
    fields = stdout.splitlines()[0].split()
    values = stdout.splitlines()[1].split()

    dict_of_vals = {}
    for ind, f in enumerate(fields):
        dict_of_vals[f.strip()] = values[ind].strip()

    # print text size, data size, then percentage full
    print "\tText:  {: >5} bytes".format(dict_of_vals["text"])
    print "\tData:  {: >5} bytes".format(dict_of_vals["data"])
    print "\t bss:  {: >5} bytes".format(dict_of_vals["bss"])
    percentage = (float(dict_of_vals["dec"]) * 100.0) / float(chip_total_flash)
    sys.stdout.write(bcolors.COLOR_WHITE)
    print "\tTotal: {: >5} / {} ({:0.2f} %)\n".format(dict_of_vals["dec"],
                                                      chip_total_flash,
                                                      percentage)
    sys.stdout.write(bcolors.COLOR_NC)


def build_get_section_info(build_type, chip_total_flash):
    chip_total_flash = chip_total_flash * 1024
    sys.stdout.write(bcolors.COLOR_YELLOW)
    b = build_type.upper()
    print("\n———————————— {} FUNCTION SIZES ———————————————\n".format(b))
    sys.stdout.write(bcolors.COLOR_NC)
    sys.stdout.flush()
    cmd = "arm-none-eabi-nm --print-size --size-sort --radix=d " + \
        "--defined-only {}/motor_driver.elf".format(build_type.lower())

    stdout, stderr, retval = run_command(cmd, print_output=False)

    data = []
    for line in stdout.splitlines():
        sym_ref, size, sym_type, name = line.split()
        data.append((name, sym_type, int(size)))

    # sort the data by size in reverse order
    data_sorted = sorted(data, key=lambda d: d[2], reverse=True)
    for val in data_sorted:
        name, sym_type, size = val
        sys.stdout.write(bcolors.COLOR_WHITE)
        sys.stdout.write(name)
        sys.stdout.write(bcolors.COLOR_NC)
        sys.stdout.flush()
        percentage = (float(size) * 100.0) / float(chip_total_flash)
        print(" - {} bytes ({:0.2}%)".format(size, percentage))


def run_command(cmd, print_output=True):
    """
    Kicks off a subprocess to run and accumilate the stdout of the process.
    """
    def enqueue_output(out, queue):
        for line in iter(out.readline, b''):
            queue.put(line)
        out.close()

    print(" -> {}".format(cmd))
    proc = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE,
                            stderr=subprocess.PIPE)
    q_stdout = Queue()
    q_stderr = Queue()
    t_stdout = Thread(target=enqueue_output, args=(proc.stdout, q_stdout))
    t_stderr = Thread(target=enqueue_output, args=(proc.stderr, q_stderr))
    t_stderr.daemon = True  # thread dies with the program
    t_stdout.daemon = True
    t_stdout.start()
    t_stderr.start()
    stdout = ""
    stderr = ""

    # read stdout and stderr without blocking
    finished = False
    while True:
        done = proc.poll()
        try:
            line_stdout = ""
            while True:
                line_stdout += q_stdout.get_nowait()  # or q.get(timeout=.1)
        except Empty:
            pass
        # accumilate stdout and print if we should
        stdout += line_stdout
        if print_output and line_stdout != "":
            print(line_stdout.rstrip("\n"))

        try:
            line_stderr = ""
            while True:
                line_stderr += q_stderr.get_nowait()  # or q.get(timeout=.1)
        except Empty:
            pass
        # accumilate stderr and print if we should
        stderr += line_stderr
        if print_output and line_stderr != "":
            print(line_stderr.rstrip("\n"))

        if finished:
            return stdout, stderr, done

        if done is not None:
            finished = True
            # give stdout and stderr time
            time.sleep(0.25)


if __name__ == '__main__':
    import argparse

    parser = argparse.ArgumentParser()
    parser.add_argument("build", type=str,
                        help="Type of build to run. Options are: "
                             "'debug', 'release', 'all', and 'clean'.")
    parser.add_argument("-f", "--flash-size", type=int, default=32,
                        help="total flash size available on the "
                             "chip in Kibibytes. Defaults to 32k.")
    parser.add_argument("-s", "--show-usage", action="store_true",
                        help="Outputs size information for individual symbols "
                        "in the builds output elf file.")
    parser.add_argument("-v", "--verbose", action="store_true",
                       help="Shows the individual calls made by make.")
    parser.add_argument("-g", "--gcc-dir", type=str, default="",
                        help="Directory for armgcc compiler.")
    parser.add_argument("-c", "--clang", action="store_true",
                        help="Use the clang compiler instead of armgcc.")

    args = parser.parse_args()
    build = args.build.strip().lower()

    build_clean(remove_elf_files=True)

    if build == "debug" or build == "all":
        retval = build_debug(args.clang, args.gcc_dir, args.verbose)
        build_clean(remove_elf_files=False)
        if retval == 0:
            build_display_size(build_type="debug",
                               chip_total_flash=args.flash_size)
            if args.show_usage:
                build_get_section_info("debug", args.flash_size)

    if build == "release" or build == "all":
        retval = build_release(args.clang, args.gcc_dir, args.verbose)
        build_clean(remove_elf_files=False)
        if retval == 0:
            build_display_size(build_type="release",
                               chip_total_flash=args.flash_size)
            if args.show_usage:
                build_get_section_info("release", args.flash_size)
