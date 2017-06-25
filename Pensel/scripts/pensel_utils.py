#!/usr/bin/env python
# -*- coding: UTF-8 -*-
import sys
import time
import struct

import subprocess
from threading import Thread
from Queue import Queue, Empty


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


def parse_accel_packet(payload):
    packed_data = struct.pack("BBBBBBBBBBBBBBBBBBBB", *payload)
    frame_num, timestamp, x, y, z = struct.unpack("IIfff", packed_data)
    return frame_num, timestamp, x, y, z


def parse_mag_packet(payload):
    packed_data = struct.pack("BBBBBBBBBBBBBBBBBBBB", *payload)
    frame_num, timestamp, x, y, z = struct.unpack("IIfff", packed_data)
    return frame_num, timestamp, x, y, z


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
            sys.stdout.write(bcolors.COLOR_CYAN)
            for line in line_stdout.splitlines():
                sys.stdout.write("\t{}\n".format(line))
            sys.stdout.write(bcolors.COLOR_NC)
            sys.stdout.flush()

        try:
            line_stderr = ""
            while True:
                line_stderr += q_stderr.get_nowait()  # or q.get(timeout=.1)
        except Empty:
            pass
        # accumilate stderr and print if we should
        stderr += line_stderr
        if print_output and line_stderr != "":
            sys.stderr.write(bcolors.COLOR_RED)
            for line in line_stderr.splitlines():
                sys.stderr.write("\t{}\n".format(line))
            sys.stderr.write(bcolors.COLOR_NC)
            sys.stderr.flush()

        # check if we're done and the finished flag is set
        if finished:
            if done != 0 and print_output is False:
                sys.stderr.write(bcolors.COLOR_RED)
                for line in stderr.splitlines():
                    sys.stderr.write("\t{}\n".format(line))
                sys.stderr.write(bcolors.COLOR_NC)
                sys.stderr.flush()

            return stdout, stderr, done

        # check if the process is done...
        if done is not None:
            finished = True
            # give the process's stdout and stderr time to flush
            time.sleep(0.25)
