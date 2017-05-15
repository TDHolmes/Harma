#!/usr/bin/env python
# -*- coding: UTF-8 -*-
import sys
import os
import subprocess
from threading import Thread
from Queue import Queue, Empty
import time


section_name_explainations = {
    'a': "The symbol's value is absolute, and will not be changed by further linking",
    'b': "The symbol is in the uninitialized data section (known as BSS)",
    'c': "The symbol is common. Common symbols are uninitialized data. When linking, multiple common symbols may "
    "appear with the same name. If the symbol is defined anywhere, the common symbols are treated as undefined "
    "references. For more details on common symbols, see the discussion of –warn-common in Linker options",
    'd': "The symbol is in the initialized data section",
    'g': "The symbol is in an initialized data section for small objects. Some object file formats permit more "
    "efficient access to small data objects, such as a global int variable as opposed to a large global array",
    'i': "The symbol is an indirect reference to another symbol",
    'n': "The symbol is a debugging symbol",
    'p': "The symbols is in a stack unwind section",
    'r': "The symbol is in a read only data section",
    's': "The symbol is in an uninitialized data section for small objects",
    't': "The symbol is in the text (code) section",
    'u': "The symbol is undefined or a unique global symbol. This is a GNU extension to the standard set of ELF "
    "symbol bindings. For such a symbol the dynamic linker will make sure that in the entire process there is just "
    "one symbol with this name and type in use",
    'v': "The symbol is a weak object. When a weak defined symbol is linked with a normal defined symbol, the normal "
    "defined symbol is used with no error. When a weak undefined symbol is linked and the symbol is not defined, the"
    " value of the weak symbol becomes zero with no error. On some systems, uppercase indicates that a default value"
    " has been specified",
    'w': "The symbol is a weak symbol that has not been specifically tagged as a weak object symbol. When a weak"
    " defined symbol is linked with a normal defined symbol, the normal defined symbol is used with no error. When a"
    " weak undefined symbol is linked and the symbol is not defined, the value of the symbol is determined in a "
    "system-specific manner without error. On some systems uppercase indicates that a default value has been specified",
    '-': "The symbol is a stabs symbol in an a.out object file. In this case, the next values printed are the stabs "
    "other field, the stabs desc field, and the stab type. Stabs symbols are used to hold debugging information",
    '?': "The symbol type is unknown, or object file format specific"}


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


def build_display_size(build_type, chip_total_flash, chip_total_ram):
    """
    Expected output from the arm toolchain size function:
    text    data     bss     dec     hex filename
    6772     112     664    7548    1d7c debug/pensel.elf

    Adds in a percentage measurement.
    """
    chip_total_flash = chip_total_flash * 1024
    chip_total_ram = chip_total_ram * 1024
    sys.stdout.write(bcolors.COLOR_YELLOW)
    b = build_type.upper()
    print("\n———————————— {} BUILD SIZE ———————————————\n".format(b))
    sys.stdout.write(bcolors.COLOR_NC)
    sys.stdout.flush()
    file_location = "build/{}/pensel.elf".format(build_type)
    cmd = "arm-none-eabi-size {} --radix=10".format(file_location)
    stdout, stderr, retval = run_command(cmd, print_output=False)
    fields = stdout.splitlines()[0].split()
    values = stdout.splitlines()[1].split()

    dict_of_vals = {}
    for ind, f in enumerate(fields):
        try:
            dict_of_vals[f.strip()] = int(values[ind].strip(), 10)
        except Exception:
            pass

    # print text size, data size, then percentage full
    print "\tText:  {: >5} bytes".format(dict_of_vals["text"])
    print "\tData:  {: >5} bytes".format(dict_of_vals["data"])
    print "\t bss:  {: >5} bytes".format(dict_of_vals["bss"])
    percentage = (float(dict_of_vals["text"]) * 100.0) / float(chip_total_flash)
    sys.stdout.write(bcolors.COLOR_WHITE)
    print "\tFlash: {: >5} / {} ({:0.2f} %)".format(dict_of_vals["text"],
                                                    chip_total_flash,
                                                    percentage)

    ram_stuff = dict_of_vals["data"] + dict_of_vals["bss"]
    percentage = (float(ram_stuff) * 100.0) / float(chip_total_ram)
    print "\t  Ram: {: >5} / {} ({:0.2f} %)\n".format(ram_stuff,
                                                      chip_total_ram,
                                                      percentage)
    sys.stdout.write(bcolors.COLOR_NC)


def build_get_section_info(build_type, chip_total_flash, chip_total_ram):
    chip_total_flash = chip_total_flash * 1024
    chip_total_ram = chip_total_ram * 1024
    sys.stdout.write(bcolors.COLOR_YELLOW)
    b = build_type.upper()
    print("\n———————————— {} FUNCTION SIZES ———————————————\n".format(b))
    sys.stdout.write(bcolors.COLOR_NC)
    sys.stdout.flush()
    cmd = "arm-none-eabi-nm --print-size --size-sort --radix=d --demangle " + \
        "--defined-only {}/pensel.elf".format(build_type.lower())

    stdout, stderr, retval = run_command(cmd, print_output=False)

    data = []
    data_by_sections = {}
    for line in stdout.splitlines():
        sym_ref, size, sym_type, name = line.split()
        try:
            sym_type = sym_type.lower()
        except Exception:
            pass
        data.append((name, sym_type, int(size)))
        if sym_type not in data_by_sections.keys():
            data_by_sections[sym_type] = []
        data_by_sections[sym_type].append((name, int(size)))

    for key in data_by_sections:
        data = data_by_sections[key]
        data_sorted = sorted(data, key=lambda d: d[1], reverse=True)

        print("\nSection {}:".format(key))
        print("\t{}".format(section_name_explainations[key]))
        for d in data_sorted:
            name, size = d
            sys.stdout.write(bcolors.COLOR_WHITE)
            sys.stdout.write(name)
            sys.stdout.write(bcolors.COLOR_NC)
            sys.stdout.flush()
            if key == "r" or key == "b" or key == "d" or key == "g" or key == "c":
                percentage = (float(size) * 100.0) / float(chip_total_ram)
                print(" - {} bytes ({:0.2f}% of RAM)".format(size, percentage))
            else:
                percentage = (float(size) * 100.0) / float(chip_total_flash)
                print(" - {} bytes ({:0.2f}% of Flash)".format(size, percentage))


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
    parser.add_argument("-f", "--flash-size", type=int, default=64,
                        help="total flash size available on the "
                             "chip in Kibibytes. Defaults to 64k.")
    parser.add_argument("-r", "--ram-size", type=int, default=16,
                        help="total ram size available on the "
                             "chip in Kibibytes. Defaults to 16k.")
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

    os.chdir("../firmware/compiler")
    # make sure all our folder paths exist
    current_path = os.path.abspath("./")
    if not os.path.exists(os.path.join(current_path, "build")):
        os.mkdir(os.path.join(current_path, "build"))

    if not os.path.exists(os.path.join(current_path, "build/Release")):
        os.mkdir(os.path.join(current_path, "build/Release"))

    if not os.path.exists(os.path.join(current_path, "build/Debug")):
        os.mkdir(os.path.join(current_path, "build/Debug"))

    if build == "debug" or build == "all":
        retval = build_debug(args.clang, args.gcc_dir, args.verbose)
        build_clean(remove_elf_files=False)
        if retval == 0:
            build_display_size(build_type="Debug",
                               chip_total_flash=args.flash_size,
                               chip_total_ram=args.ram_size)
            if args.show_usage:
                build_get_section_info("Debug", args.flash_size, args.ram_size)

    if build == "release" or build == "all":
        retval = build_release(args.clang, args.gcc_dir, args.verbose)
        build_clean(remove_elf_files=False)
        if retval == 0:
            build_display_size(build_type="Release",
                               chip_total_flash=args.flash_size,
                               chip_total_ram=args.ram_size)
            if args.show_usage:
                build_get_section_info("Release", args.flash_size, args.ram_size)
