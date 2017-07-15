#! /usr/bin/python
import os
import threading
import Queue

import pensel_utils as pu
import penselreport as pr


class PenselInputs(pr.Pensel):
    partial_pkt = []

    def __init__(self, *args, **kwargs):
        super(PenselInputs, self).__init__(*args, **kwargs)
        self.start_listener()
        self._clear_serial = False

    def start_listener(self):
        self.thread_run = True
        self.queue = Queue.Queue()
        self.thread = threading.Thread(target=self.listener)  # , args=(self,)
        self.thread.start()  # start it off

    def run_parser(self):
        if self.check_for_start():
            report, retval, payload = self.receive_packet()
            if report >= 0 and retval == 0:
                self.queue.put((report, retval, payload))
                return True

        return False

    def listener(self):
        while self.thread_run is True:
            self.run_parser()
            if self._clear_serial:
                self.serial_clear()  # clear old serial data that we don't care about :P
                self._clear_serial = False

    def get_packet(self):
        if self.thread.is_alive() is False:
            raise RuntimeError("Thread is dead!!")
        # elif self.verbose:
        #     print("Thread is still alive")

        if not self.queue.empty():
            try:
                return self.queue.get_nowait()
            except Queue.Empty:
                pass
        else:
            # queue is empty...
            pass
        return None

    def clear_queue(self):
        while True:
            try:
                self.queue.get_nowait()
            except Queue.Empty:
                if self.verbose:
                    print("Queue cleared!")
                break

    def close(self):
        super(PenselInputs, self).close()
        print("Killing thread")
        self.thread_run = False


def parse_inputreport(reportID, payload):
    if reportID == 0x81:
        # typedef struct __attribute__((packed)) {
        #     uint32_t frame_num;   //!< Frame number
        #     uint32_t timestamp;  //!< Timestamp when the packet was received
        #     float x;             //!< Accel X value
        #     float y;             //!< Accel Y value
        #     float z;             //!< Accel Z value
        # } accel_norm_t;
        frame_num, timestamp, x, y, z = pu.parse_accel_packet(payload)
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
        frame_num, timestamp, x, y, z = pu.parse_mag_packet(payload)
        print("   Mag Packet:")
        print("      Frame #: {}".format(frame_num))
        print("    Timestamp: {} ms".format(timestamp))
        print("       X Axis: {}".format(x))
        print("       Y Axis: {}".format(y))
        print("       Z Axis: {}".format(z))


if __name__ == '__main__':
    import argparse
    parser = argparse.ArgumentParser(description='Receive input data from the Pensel.')
    parser.add_argument('--baudrate', "-b", dest='baudrate', default=115200,
                        help='Baudrate of the serial data to recieve')
    parser.add_argument('--parsed', default=False, action="store_true",
                        help='Whether or not we should print a parsed version of the report')
    parser.add_argument("--reports", nargs='*', type=str,
                        help="reports to stream (filter out ones we don't care about)")
    parser.add_argument('--verbose', default=False, action="store_true",
                        help='Whether or not we should print extra debug information')

    args = parser.parse_args()

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
                choice = int(raw_input("Which port do you choose? "))
            except Exception:
                print("Invalid choice.")
                continue

            if choice < len(list_of_ports):
                return list_of_ports[choice]
            else:
                print("Invalid index.")

    # port = "/dev/" + choose_port(find_ports())
    port = "/dev/cu.SLAB_USBtoUART"

    reports_to_stream = [int(r, 16) for r in args.reports]

    with PenselInputs(port, args.baudrate, args.verbose) as pi:
        while True:
            pkt = pi.get_packet()
            if pkt:
                report, retval, payload = pkt
                if len(reports_to_stream) == 0 or report in reports_to_stream:
                    if retval == 0:
                        if args.parsed:
                            parse_inputreport(report, payload)
                        else:
                            print(pkt)
                    else:
                        print("ERROR: report {} returned retval {}".format(report, retval))
                elif args.verbose:
                    print("Ignoring report ID {}".format(report))
