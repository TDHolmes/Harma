#!/usr/bin/env python3
# -*- coding: UTF-8 -*-
import sys
import time
import struct
import serial
import threading

from datetime import datetime
from threading import Thread, Event

if sys.version_info.major == 2:
    import subprocess32 as subprocess
    from Queue import Queue as queue
    from Queue import Empty
else:
    import subprocess
    from queue import Queue as queue
    from queue import Empty


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


class Pensel(object):

    MAGIC_NUM_0 = 0xDE
    MAGIC_NUM_1 = 0xAD
    MAGIC_NUM_2 = 0xBE
    MAGIC_NUM_3 = 0xEF
    MAGIC_HEADER = [MAGIC_NUM_0, MAGIC_NUM_1, MAGIC_NUM_2, MAGIC_NUM_3]

    _default_baud = 250000

    def __init__(self, serialport, baudrate, verbose=False, timeout=1):
        self.serialport = serialport
        self.TIMEOUT = timeout
        self.baudrate = baudrate or self._default_baud
        self.verbose = verbose
        self.log_lock = threading.Lock()
        self._check_for_start_bytes = []

        # open serial port
        if self.verbose:
            self.log("Opening serial port...")
        self.serial = serial.Serial(self.serialport, self.baudrate, timeout=0.5)
        if self.verbose:
            self.log("Opened!")

        # start listening for reports from Pensel
        self.start_listener()
        self._clear_serial = False

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        if self.verbose:
            self.log("Cleaning up...")
        self.close()

    def log(self, string):
        with self.log_lock:
            prepend = datetime.now().strftime("%H:%M:%S.%f ")
            print(prepend + string)

    def serial_write(self, values_to_write):
        """
        Writes `values_to_write` to the serial port.
        """
        if self.verbose:
            self.log("Writing 0x{:x} to serial port...".format(values_to_write))
        if type(values_to_write) is not list:
            self.serial.write(bytearray([values_to_write]))
        else:
            self.serial.write(bytearray(values_to_write))

    def serial_read(self, num_bytes):
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

    def serial_clear(self):
        """ Clears the serial buffer of anything received. """
        self.serial.reset_input_buffer()

    def serial_bytes_available(self):
        """
        Returns the number of bytes in the input buffer.
        """
        return self.serial.in_waiting

    def start_listener(self):
        self.thread_run = Event()
        self.thread_run.set()
        self.queue = queue()
        self.thread = threading.Thread(target=self.listener)  # , args=(self,)
        self.thread.start()  # start it off

    def packets_available(self):
        return not self.queue.empty()

    def listener(self):
        """ The threaded listener that looks for packets from Pensel. """
        while self.thread_run.is_set():
            if self.serial_bytes_available() >= len(self.MAGIC_HEADER) and \
                    self.check_for_start():
                report, retval, payload = self.receive_packet()
                if report >= 0:
                    self.queue.put((report, retval, payload))
                    if self.verbose:
                        self.log("Put report {} on queue".format(report))

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
            except queue.Empty:
                if self.verbose:
                    self.log("Queue cleared!")
                break

    def send_report(self, report_ID, payload=None):
        """
        Sends a report to the pensel and reads back the result
        """
        if report_ID < 0 or report_ID > 127:
            raise ValueError("Report ID {} is out of the valid range!".format(report_ID))

        self.serial_write(self.MAGIC_NUM_0)
        self.serial_write(self.MAGIC_NUM_1)
        self.serial_write(self.MAGIC_NUM_2)
        self.serial_write(self.MAGIC_NUM_3)
        self.serial_write(report_ID)
        _bytes = [self.MAGIC_NUM_0, self.MAGIC_NUM_1, self.MAGIC_NUM_2, self.MAGIC_NUM_3, report_ID]
        if payload is None:
            _bytes.append(0)
            self.serial_write(0)
        else:
            _bytes.append(len(payload))
            self.serial_write(len(payload))
            for b in payload:
                if b < 0 or b > 255:
                    raise ValueError("Value in payload out of valid range!")
                _bytes.append(b)
                self.serial_write(b)
        # Checksum time!
        self.serial_write(generate_checksum(_bytes))

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

    def check_for_start(self):
        """
        Checks for the start of a reply from Pensel.
        """
        while self.serial_bytes_available():
            data = self.serial_read(1)
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

    def receive_packet(self):
        """
        Receives a packet, whether from a report reply or an input report,
        from Pensel. Doesn't check for start of packet. That's `check_for_start`
        """
        report = self.serial_read(1)
        if len(report) != 1:
            self.log("ERROR: Didn't read back a report!")
            report = -1
        else:
            report = report[0]
        retval = self.serial_read(1)
        if len(retval) != 1:
            self.log("ERROR: Didn't read back a return value!")
            retval = -1
        else:
            retval = retval[0]

        return_payload_len = self.serial_read(1)
        if len(return_payload_len) != 1:
            self.log("ERROR: Didn't read back a return payload length!")
            return_payload_len = 0
        else:
            return_payload_len = return_payload_len[0]

        if return_payload_len != 0:
            return_payload = self.serial_read(return_payload_len)
        else:
            return_payload = []
        checksum = self.serial_read(1)
        if len(checksum) != 1:
            self.log("ERROR: Didn't read back a checksum!")
            checksum = -1
        else:
            checksum = checksum[0]

        data = self.MAGIC_HEADER + [report, retval, return_payload_len] + return_payload
        data.append(checksum)

        our_checksum = generate_checksum(data[:-1])
        if our_checksum != checksum:
            self.log("ERROR: Our checksum didn't calculate properly! "
                     "(Calculated {}, expected {})".format(our_checksum, checksum))
            return -1, checksum, []
        else:
            if self.verbose:
                self.log("Checksum match! ({} == {})".format(our_checksum, checksum))

        return report, retval, return_payload

    def close(self):
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


def generate_checksum(list_of_data):
    checksum = 0
    for b in list_of_data:
        checksum = (checksum + b) & 0xFF
    checksum = (256 - checksum) & 0xff  # twos complement of a byte
    return checksum


def parse_report(reportID, payload):
    if reportID == 0x22:
        # typedef struct __attribute__((packed)) {
        #     uint32_t frame_num;   //!< Frame number
        #     uint32_t timestamp;  //!< Timestamp when the packet was received
        #     float x;             //!< Accel X value
        #     float y;             //!< Accel Y value
        #     float z;             //!< Accel Z value
        # } accel_norm_t;
        frame_num, timestamp, x, y, z = parse_accel_packet(payload)
        print("   Accel Packet:")
        print("       Frame #: {}".format(frame_num))
        print("     Timestamp: {} ms".format(timestamp))
        print("        X Axis: {}".format(x))
        print("        Y Axis: {}".format(y))
        print("        Z Axis: {}".format(z))

    elif reportID == 0x23:
        # typedef struct __attribute__((packed)) {
        #     uint32_t frame_num;   //!< Frame number
        #     uint32_t timestamp;  //!< Timestamp when the packet was received
        #     float x;             //!< Mag X value
        #     float y;             //!< Mag Y value
        #     float z;             //!< Mag Z value
        # } mag_norm_t;
        frame_num, timestamp, x, y, z = parse_mag_packet(payload)
        print("   Mag Packet:")
        print("      Frame #: {}".format(frame_num))
        print("    Timestamp: {} ms".format(timestamp))
        print("       X Axis: {}".format(x))
        print("       Y Axis: {}".format(y))
        print("       Z Axis: {}".format(z))

    elif reportID == 0x24:
        packed_data = struct.pack("BBBBBBBBBBBBBBBB", *payload)
        accel_pkt_ovrwt, mag_pkt_ovrwt, accel_hw_ovrwt, mag_hw_ovrwt = struct.unpack("=IIII", packed_data)
        print("   LSM303DLHC errors:")
        print("     Accel Packet Overwrites: {}".format(accel_pkt_ovrwt))
        print("       Mag Packet Overwrites: {}".format(mag_pkt_ovrwt))
        print("   Accel Hardware Overwrites: {}".format(accel_hw_ovrwt))
        print("     Mag Hardware Overwrites: {}".format(mag_hw_ovrwt))

    elif reportID == 0x28 or reportID == 0x29 or reportID == 0x2A:
        packed_data = struct.pack("BBBBBBBBBBBB", *payload)
        x, y, z = struct.unpack("fff", packed_data)
        print("<{}, {}, {}>".format(x, y, z))

    elif reportID == 0x30:
        # Pensel Version
        print("    Pensel v{}.{}".format(payload[0], payload[1]))

    elif reportID == 0x31:
        packed_data = struct.pack("BBBB", *payload)
        timestamp = struct.unpack("I", packed_data)[0]
        print("    Timestamp: {} ms".format(timestamp))

    elif reportID == 0x33:
        print("    Switch State: {}".format(payload[0]))
        print("     Main Button: {}".format("open" if payload[1] else "pressed"))
        print("      Aux Button: {}".format("open" if payload[2] else "pressed"))

    elif reportID == 0x34:
        packed_data = struct.pack("BBBBBBBBBBBBB", *payload)
        bitfields, dropped, dequeued, queued = struct.unpack("<BIII", packed_data)

        print("bitfields: \t{}".format(hex(bitfields)))
        print("dropped: \t{:,}".format(dropped))
        print("dequeued: \t{:,}".format(dequeued))
        print("queued: \t{:,}".format(queued))

    else:
        print("    {}".format(" ".join(["{:0>2X}".format(b) for b in payload])))


def parse_inputreport(reportID, payload):
    if reportID == 0x81:
        # typedef struct __attribute__((packed)) {
        #     uint32_t frame_num;   //!< Frame number
        #     uint32_t timestamp;  //!< Timestamp when the packet was received
        #     float x;             //!< Accel X value
        #     float y;             //!< Accel Y value
        #     float z;             //!< Accel Z value
        # } accel_norm_t;
        frame_num, timestamp, x, y, z = parse_accel_packet(payload)
        print("   Accel Packet:")
        print("       Frame #: {}".format(frame_num))
        print("     Timestamp: {} ms".format(timestamp))
        print("        X Axis: {}".format(x))
        print("        Y Axis: {}".format(y))
        print("        Z Axis: {}".format(z))

    elif reportID == 0x82:
        # typedef struct __attribute__((packed)) {
        #     uint32_t frame_num;   //!< Frame number
        #     uint32_t timestamp;  //!< Timestamp when the packet was received
        #     float x;             //!< Mag X value
        #     float y;             //!< Mag Y value
        #     float z;             //!< Mag Z value
        # } mag_norm_t;
        frame_num, timestamp, x, y, z = parse_mag_packet(payload)
        print("   Mag Packet:")
        print("      Frame #: {}".format(frame_num))
        print("    Timestamp: {} ms".format(timestamp))
        print("       X Axis: {}".format(x))
        print("       Y Axis: {}".format(y))
        print("       Z Axis: {}".format(z))

    elif reportID == 0x83:
        frame_num, timestamp, x, y, z = parse_accel_packet(payload)
        print("Filtered Accel Packet:")
        print("      Frame #: {}".format(frame_num))
        print("    Timestamp: {} ms".format(timestamp))
        print("       X Axis: {}".format(x))
        print("       Y Axis: {}".format(y))
        print("       Z Axis: {}".format(z))

    elif reportID == 0x84:
        frame_num, timestamp, x, y, z = parse_mag_packet(payload)
        print("Filtered Mag Packet:")
        print("      Frame #: {}".format(frame_num))
        print("    Timestamp: {} ms".format(timestamp))
        print("       X Axis: {}".format(x))
        print("       Y Axis: {}".format(y))
        print("       Z Axis: {}".format(z))


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
    q_stdout = queue()
    q_stderr = queue()
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
