#!/usr/bin/env python3
# -*- coding: UTF-8 -*-
import os
import sys
import abc
import time
import struct
import serial
import threading
import collections as col
from types import SimpleNamespace

from datetime import datetime
from threading import Thread, Event

if sys.version_info.major == 2:
    import subprocess32 as subprocess
    from Queue import Queue, Empty
else:
    import subprocess
    from queue import Queue, Empty


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


# Pensel comms object

class PenselError(RuntimeError):
    pass


class BasePensel(object, metaclass=abc.ABCMeta):

    def __init__(self, *args, verbose=False, **kwargs):
        self.verbose = verbose

    # ------ Common Methods ----- #

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        if self.verbose:
            self.log("Cleaning up...")
        self.close(exc_type, exc_val, exc_tb)

    def log(self, string):
        with self.log_lock:
            prepend = datetime.now().strftime("%H:%M:%S.%f ")
            print(prepend + string)

    @staticmethod
    def generate_checksum(list_of_data):
        checksum = 0
        for b in list_of_data:
            checksum = (checksum + b) & 0xFF
        checksum = (256 - checksum) & 0xff  # twos complement of a byte
        return checksum

    def parse_report(self, reportID, payload, verbose=True):
        # Pack the data to be used in some of the parsing functions
        packed_data = struct.pack("B" * len(payload), *payload)

        if reportID == 0x22:
            pkt = self.parse_accel_packet(payload)
            if verbose:
                print("   Accel Packet:")
                print("       Frame #: {}".format(pkt.frame_num))
                print("     Timestamp: {} ms".format(pkt.timestamp))
                print("        X Axis: {}".format(pkt.x))
                print("        Y Axis: {}".format(pkt.y))
                print("        Z Axis: {}".format(pkt.z))
                print("")
            return pkt

        elif reportID == 0x23:
            pkt = self.parse_mag_packet(payload)
            if verbose:
                print("   Mag Packet:")
                print("      Frame #: {}".format(pkt.frame_num))
                print("    Timestamp: {} ms".format(pkt.timestamp))
                print("       X Axis: {}".format(pkt.x))
                print("       Y Axis: {}".format(pkt.y))
                print("       Z Axis: {}".format(pkt.z))
                print("")
            return pkt

        elif reportID == 0x24:
            data = struct.unpack("=IIII", packed_data)
            p = SimpleNamespace(
                accel_pkt_ovrwt=data[0], mag_pkt_ovrwt=data[1],
                accel_hw_ovrwt=data[2], mag_hw_ovrwt=data[3])
            if verbose:
                print("   LSM303DLHC errors:")
                print("     Accel Packet Overwrites: {}".format(p.accel_pkt_ovrwt))
                print("       Mag Packet Overwrites: {}".format(p.mag_pkt_ovrwt))
                print("   Accel Hardware Overwrites: {}".format(p.accel_hw_ovrwt))
                print("     Mag Hardware Overwrites: {}".format(p.mag_hw_ovrwt))
            return p

        elif reportID == 0x28 or reportID == 0x29 or reportID == 0x2A:
            data = struct.unpack("fff", packed_data)
            p = SimpleNamespace(x=data[0], y=data[1], z=data[2])
            if verbose:
                print("<{}, {}, {}>".format(p.x, p.y, p.z))
            return p

        elif reportID == 0x30:
            # Pensel Version
            pkt = col.namedtuple("Version", ["major", "minor", "git_hash"])
            p = pkt(*struct.unpack("=BBI", packed_data))
            if verbose:
                print("    Pensel v{}.{}-{}".format(p.major, p.minor, p.git_hash))
            return p

        elif reportID == 0x31:
            # current timestamp
            p = struct.unpack("I", packed_data)[0]
            if verbose:
                print("    Timestamp: {} ms".format(p))
            return p

        elif reportID == 0x33:
            # Switch/button states
            pkt = col.namedtuple("ButtonState", ["switch", "main", "aux"])
            p = pkt(*struct.unpack("=BBB", packed_data))
            if verbose:
                print("    Switch State: {}".format(p.switch))
                print("     Main Button: {}".format("open" if p.main else "pressed"))
                print("      Aux Button: {}".format("open" if p.aux else "pressed"))
            return p

        elif reportID == 0x34:
            #
            pkt = col.namedtuple("ButtonState", ["bitfields", "dropped", "dequeued", "queued"])
            p = pkt(*struct.unpack("<BIII", packed_data))
            if verbose:
                print("bitfields: \t{}".format(hex(p.bitfields)))
                print("dropped: \t{:,}".format(p.dropped))
                print("dequeued: \t{:,}".format(p.dequeued))
                print("queued: \t{:,}".format(p.queued))
            return p

        elif reportID == 0x81:
            pkt = self.parse_accel_packet(packed_data)
            if verbose:
                print("   Accel Packet:")
                print("       Frame #: {}".format(pkt.frame_num))
                print("     Timestamp: {} ms".format(pkt.timestamp))
                print("        X Axis: {}".format(pkt.x))
                print("        Y Axis: {}".format(pkt.y))
                print("        Z Axis: {}".format(pkt.z))
                print("")
            return pkt

        elif reportID == 0x82:
            pkt = self.parse_mag_packet(packed_data)
            if verbose:
                print("   Mag Packet:")
                print("      Frame #: {}".format(pkt.frame_num))
                print("    Timestamp: {} ms".format(pkt.timestamp))
                print("       X Axis: {}".format(pkt.x))
                print("       Y Axis: {}".format(pkt.y))
                print("       Z Axis: {}".format(pkt.z))
                print("")
            return pkt

        elif reportID == 0x83:
            pkt = self.parse_accel_packet(packed_data)
            if verbose:
                print("Filtered Accel Packet:")
                print("      Frame #: {}".format(pkt.frame_num))
                print("    Timestamp: {} ms".format(pkt.timestamp))
                print("       X Axis: {}".format(pkt.x))
                print("       Y Axis: {}".format(pkt.y))
                print("       Z Axis: {}".format(pkt.z))
                print("")
            return pkt

        elif reportID == 0x84:
            pkt = self.parse_mag_packet(packed_data)
            if verbose:
                print("Filtered Mag Packet:")
                print("      Frame #: {}".format(pkt.frame_num))
                print("    Timestamp: {} ms".format(pkt.timestamp))
                print("       X Axis: {}".format(pkt.x))
                print("       Y Axis: {}".format(pkt.y))
                print("       Z Axis: {}".format(pkt.z))
                print("")
            return pkt

    @staticmethod
    def parse_accel_packet(packed_data):
        frame_num, timestamp, x, y, z = struct.unpack("IIfff", packed_data)
        return SimpleNamespace(x=x, y=y, z=z, frame_num=frame_num, timestamp=timestamp)

    @staticmethod
    def parse_mag_packet(packed_data):
        frame_num, timestamp, x, y, z = struct.unpack("IIfff", packed_data)
        return SimpleNamespace(x=x, y=y, z=z, frame_num=frame_num, timestamp=timestamp)

    def get_packet(self):
        if self.thread.is_alive() is False:
            raise RuntimeError("Thread is dead!!")

        # if not self.queue.empty():
        try:
            return self.queue.get(timeout=0.01)
        except Empty:
            pass

        return None

    def get_packet_withreportID(self, reportID, timeout=0.01):
        """
        Returns the first found packet with the given reportID while keeping
        the rest of the packets on the queue in the correct order.
        """
        # check if we've got a correct packet in the queue
        incorrect_packets = []
        correct_pkt = None
        start_time = time.time()
        while time.time() < start_time + timeout:
            pkt = self.get_packet()
            if pkt:
                report, retval, payload = pkt
                # check if it's the correct report
                if reportID == report:
                    correct_pkt = pkt
                    break
                else:
                    # self.log("Incorrect packet type: {}".format(report))
                    incorrect_packets.append(pkt)
            else:
                time.sleep(0.001)

        # put back incorrect packets onto the queue
        for pkt in incorrect_packets:
            self.queue.put(pkt)

        return correct_pkt

    def clear_queue(self):
        while True:
            try:
                self.queue.get(timeout=0.001)
            except Empty:
                if self.verbose:
                    self.log("Queue cleared!")
                break

    def packets_available(self):
        return not self.queue.empty()

    # ----- Required Methods ------

    @abc.abstractmethod
    def send_report(self, report_ID, payload=None):
        """
        Sends a report to the pensel and reads back the result
        """
        raise NotImplementedError

    @abc.abstractmethod
    def close(self, exc_type=None, exc_val=None, exc_tb=None):
        raise NotImplementedError


class Pensel(BasePensel):

    MAGIC_NUM_0 = 0xDE
    MAGIC_NUM_1 = 0xAD
    MAGIC_NUM_2 = 0xBE
    MAGIC_NUM_3 = 0xEF
    MAGIC_HEADER = [MAGIC_NUM_0, MAGIC_NUM_1, MAGIC_NUM_2, MAGIC_NUM_3]

    _default_baud = 250000

    def __init__(self, serialport, baudrate, *args, timeout=1, **kwargs):
        super().__init__(*args, **kwargs)
        self.serialport = serialport
        self.TIMEOUT = timeout
        self.baudrate = baudrate or self._default_baud
        self.log_lock = threading.Lock()
        self._check_for_start_bytes = []

        # open serial port
        if self.verbose:
            self.log("Opening serial port...")
        self.serial = serial.Serial(self.serialport, self.baudrate, timeout=0.5)
        if self.verbose:
            self.log("Opened!")

        # start listening for reports from Pensel
        self._start_listener()
        self._clear_serial = False

    # ----- Public Methods ----- #

    def send_report(self, report_ID, payload=None):
        """
        Sends a report to the pensel and reads back the result
        """
        if report_ID < 0 or report_ID > 127:
            raise ValueError("Report ID {} is out of the valid range!".format(report_ID))

        self._serial_write(self.MAGIC_NUM_0)
        self._serial_write(self.MAGIC_NUM_1)
        self._serial_write(self.MAGIC_NUM_2)
        self._serial_write(self.MAGIC_NUM_3)
        self._serial_write(report_ID)
        _bytes = [self.MAGIC_NUM_0, self.MAGIC_NUM_1, self.MAGIC_NUM_2, self.MAGIC_NUM_3, report_ID]
        if payload is None:
            _bytes.append(0)
            self._serial_write(0)
        else:
            _bytes.append(len(payload))
            self._serial_write(len(payload))
            for b in payload:
                if b < 0 or b > 255:
                    raise ValueError("Value in payload out of valid range!")
                _bytes.append(b)
                self._serial_write(b)
        # Checksum time!
        self._serial_write(self.generate_checksum(_bytes))

        # Try to get the response
        retval = None
        payload = None
        start_time = time.time()
        while time.time() - start_time < self.TIMEOUT:
            pkt = self.get_packet_withreportID(report_ID)
            if pkt:
                report, retval, payload = pkt
                break
            else:
                pass
                # self.log("Failed to get report with ID {}".format(report_ID))
        else:
            # check for timeout
            self.log("WARNING: Timed out waiting for response")

        return retval, payload

    def close(self, exc_type=None, exc_val=None, exc_tb=None):
        try:
            if self.verbose:
                self.log("Killing thread")
            self.thread_run.clear()
            # wait for it to stop
            while self.thread.is_alive():
                time.sleep(0.01)
        finally:
            if self.verbose:
                self.log("\n\tClosing serial port...\n")
            self.serial.close()

    # ----- Private Methods ----- #

    def _serial_write(self, values_to_write):
        """
        Writes `values_to_write` to the serial port.
        """
        if self.verbose:
            self.log("Writing 0x{:x} to serial port...".format(values_to_write))
        if type(values_to_write) is not list:
            self.serial.write(bytearray([values_to_write]))
        else:
            self.serial.write(bytearray(values_to_write))

    def _serial_read(self, num_bytes):
        """
        reads `num_bytes` from the serial port.
        """
        out = self.serial.read(num_bytes)
        if len(out) != num_bytes:
            self.log("WARNING: Didn't get the expected number of bytes")
            self.log("    Received {}, expected {}. Serial port dead?".format(len(out), num_bytes))

        out_list = [int(v) for v in bytearray(out)]
        if self.verbose:
            self.log("Read in: {}".format(" ".join(["{:0>2X}".format(b) for b in out_list])))

        return out_list

    def _serial_clear(self):
        """ Clears the serial buffer of anything received. """
        self.serial.reset_input_buffer()

    def _serial_bytes_available(self):
        """
        Returns the number of bytes in the input buffer.
        """
        return self.serial.in_waiting

    def _start_listener(self):
        self.thread_run = Event()
        self.thread_run.set()
        self.queue = Queue()
        self.thread = threading.Thread(target=self._listener)  # , args=(self,)
        self.thread.start()  # start it off

    def _listener(self):
        """ The threaded listener that looks for packets from Pensel. """
        while self.thread_run.is_set():
            if self._serial_bytes_available() >= len(self.MAGIC_HEADER) and \
                    self._check_for_start():
                report, retval, payload = self._receive_packet()
                if report >= 0:
                    self.queue.put((report, retval, payload))
                    if self.verbose:
                        self.log("Put report {} on queue".format(report))

    def _check_for_start(self):
        """
        Checks for the start of a reply from Pensel.
        """
        while self._serial_bytes_available():
            data = self._serial_read(1)
            if len(data) == 1:
                self._check_for_start_bytes.append(data[0])
                try:
                    if self._check_for_start_bytes[-1] == self.MAGIC_NUM_3 and \
                            self._check_for_start_bytes[-2] == self.MAGIC_NUM_2 and \
                            self._check_for_start_bytes[-3] == self.MAGIC_NUM_1 and \
                            self._check_for_start_bytes[-4] == self.MAGIC_NUM_0:

                        if self.verbose:
                            self.log("Start Detected!")
                        return True
                except IndexError:
                    pass
            else:
                break

        # default, no start :(
        if self.verbose:
            self.log("Failed to detect start...")
        return False

    def _receive_packet(self):
        """
        Receives a packet, whether from a report reply or an input report,
        from Pensel. Doesn't check for start of packet. That's `check_for_start`
        """
        report = self._serial_read(1)
        if len(report) != 1:
            self.log("ERROR: Didn't read back a report!")
            report = -1
        else:
            report = report[0]
        retval = self._serial_read(1)
        if len(retval) != 1:
            self.log("ERROR: Didn't read back a return value!")
            retval = -1
        else:
            retval = retval[0]

        return_payload_len = self._serial_read(1)
        if len(return_payload_len) != 1:
            self.log("ERROR: Didn't read back a return payload length!")
            return_payload_len = 0
        else:
            return_payload_len = return_payload_len[0]

        if return_payload_len != 0:
            return_payload = self._serial_read(return_payload_len)
        else:
            return_payload = []
        checksum = self._serial_read(1)
        if len(checksum) != 1:
            self.log("ERROR: Didn't read back a checksum!")
            checksum = -1
        else:
            checksum = checksum[0]

        data = self.MAGIC_HEADER + [report, retval, return_payload_len] + return_payload
        data.append(checksum)

        our_checksum = self.generate_checksum(data[:-1])
        if our_checksum != checksum:
            self.log("ERROR: Our checksum didn't calculate properly! "
                     "(Calculated {}, expected {})".format(our_checksum, checksum))
            return -1, checksum, []
        else:
            if self.verbose:
                self.log("Checksum match! ({} == {})".format(our_checksum, checksum))

        return report, retval, return_payload


class PenselPlayback(BasePensel):

    def __init__(self, playback_file, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.playback_file = playback_file
        self._start_listener()

    def send_report(self, report_ID, payload=None):
        raise RuntimeError("Cannot send a report to a playback file!")

    def close(self, exc_type=None, exc_val=None, exc_tb=None):
        self.thread_run.clear()

    def _start_listener(self):
        self.thread_run = Event()
        self.thread_run.set()
        self.queue = Queue()
        self.thread = threading.Thread(target=self._listener)  # , args=(self,)
        self.thread.start()  # start it off

    def _listener(self):
        """ The threaded listener that looks for packets from Pensel. """
        with open(self.playback_file, "rb") as f:
            while self.thread_run.is_set():
                length = f.read(1)
                if len(length) == 0:
                    # out of data
                    break
                length = length[0]
                data = f.read(length)
                if len(data) != length:
                    raise RuntimeError("Didn't receive the expected amount of bytes!")

                # itterating over bytes gives us ints
                report = data[0]
                retval = data[1]
                payload = [d for d in data[2:]]
                if report >= 0:
                    self.queue.put((report, retval, payload))
                    if self.verbose:
                        self.log("Put report {} on queue".format(report))

        if self.verbose:
            self.log("Waiting for queue to empty...")

        while self.packets_available():
            time.sleep(0.01)


# ----- Helpful standalone methods


def find_ports():
    ports = []
    dev_dir = os.listdir("/dev/")
    for thing in dev_dir:
        if "cu." in thing:
            ports.append(thing)

    return ports


def choose_port(list_of_ports):
    print("\nPorts:")
    for ind, port in enumerate(list_of_ports):
        print("\t{}: {}".format(ind, port))
    while True:
        try:
            choice = int(input("Which port do you choose? "))
        except Exception:
            print("Invalid choice.")
            continue

        if choice < len(list_of_ports):
            return list_of_ports[choice]
        else:
            print("Invalid index.")


def run_command(cmd, print_output=True):
    """
    Kicks off a subprocess to run and accumilate the stdout of the process.
    """
    def enqueue_output(out, queue):
        for line in iter(out.readline, b''):
            queue.put(line.decode("utf-8"))
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
                line_stdout += q_stdout.get(timeout=0.01)
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
                line_stderr += q_stderr.get(timeout=0.01)
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
