#! /usr/bin/python
import os
import struct
import time

import penselinputs as pi
import pensel_utils as pu


def crunch_mag(mag_pkts):
    """
    Function that takes in a bunch of mag packets and normalizes them to an offset
    and gain multiplier.
    """
    offsets = [0, 0, 0]
    gains = [1, 1, 1]

    # find min/max values for each axis
    min_vals = [mag_pkts[0][0], mag_pkts[0][1], mag_pkts[0][2]]
    max_vals = [mag_pkts[0][0], mag_pkts[0][1], mag_pkts[0][2]]
    for pkt in mag_packets:
        for ind, val in enumerate(pkt):
            if val < min_vals[ind]:
                min_vals[ind] = val
            if val > max_vals[ind]:
                max_vals[ind] = val

    # Based on min/max, calculate mid-point of data to be the offset
    ind = 0
    print("Min mag: {}\t\tMax mag: {}".format(min_vals, max_vals))
    for min_val, max_val in zip(min_vals, max_vals):
        offsets[ind] = float(max_val + min_val) / 2.0
        # TODO: Figure out gain calculation
        ind += 1
    return offsets, gains


def crunch_accel(accel_pkts):
    offsets = [0, 0, 0]
    gains = [1, 1, 1]

    # find min/max values for each axis
    min_vals = [accel_pkts[0][0], accel_pkts[0][1], accel_pkts[0][2]]
    max_vals = [accel_pkts[0][0], accel_pkts[0][1], accel_pkts[0][2]]
    for pkt in accel_packets:
        for ind, val in enumerate(pkt):
            if val < min_vals[ind]:
                min_vals[ind] = val
            if val > max_vals[ind]:
                max_vals[ind] = val

    # Based on min/max, calculate mid-point of data to be the offset
    ind = 0
    print("Min accel: {}\t\tMax accel: {}".format(min_vals, max_vals))
    for min_val, max_val in zip(min_vals, max_vals):
        offsets[ind] = float(max_val + min_val) / 2.0
        # TODO: Figure out gain calculation
        ind += 1
    return offsets, gains


def generate_checksum(list_of_data):
    checksum = 0
    for b in list_of_data:
        checksum = (checksum + b) & 0xFF
    checksum = 256 - checksum - 1  # twos complement of a byte
    return checksum


def generate_calBlob(mag_offsets, mag_gains, accel_offsets, accel_gains):
    PENSEL_CAL_HEADER = 0x9f5366f1   # cal header
    PENSEL_CAL_VERSION = 0x0001      # Cal version: v00.01
    args = mag_offsets + mag_gains + accel_offsets + accel_gains + [0]
    # allignment specified so no padding is added
    blob = struct.pack("<IHffffffffffffB", PENSEL_CAL_HEADER, PENSEL_CAL_VERSION, *args)
    blob = list(blob)
    blob = [ord(v) for v in blob]
    checksum = generate_checksum(blob)
    print("calculated checksum: {}".format(checksum))
    blob.append(checksum)
    return blob


if __name__ == '__main__':
    import argparse
    parser = argparse.ArgumentParser(description='Generates a cal blob after gathering data.')
    parser.add_argument('--verbose', default=False, action="store_true",
                        help='Whether or not we should print extra debug information')
    parser.add_argument('--flash', default=False, action="store_true",
                        help='If specified, flash generated cal blob to the pensel.')
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

            if choice < len(list_of_ports) and choice >= 0:
                return list_of_ports[choice]
            else:
                print("Invalid index.")

    # port = "/dev/" + choose_port(find_ports())
    port = "/dev/cu.SLAB_USBtoUART"

    accel_packets = []
    mag_packets = []
    with pi.PenselInputs(port, 115200, args.verbose) as pi:
        # turn on accel & mag streaming
        retval, response = pi.send_report(0x20, payload=[3])
        while True:
            try:
                pkt = pi.get_packet()
                if pkt:
                    report, retval, payload = pkt
                    if report == 0x81:
                        frame_num, timestamp, x, y, z = pu.parse_accel_packet(payload)
                        accel_packets.append((x, y, z))
                        print("Accel pkt# {}: {}, {}, {}".format(frame_num, x, y, z))
                    elif report == 0x82:
                        frame_num, timestamp, x, y, z = pu.parse_mag_packet(payload)
                        mag_packets.append((x, y, z))
                        print("Mag pkt# {}: {}, {}, {}".format(frame_num, x, y, z))
            except KeyboardInterrupt:
                print("\nKeyboard Interrupt. Finishing data collection")
                break

        # turn off accel & mag streaming
        retval, response = pi.send_report(0x20, payload=[0])
        if retval != 0:
            print("potentially failed to disable streaming. retval: {} response: {}".format(retval, response))
        time.sleep(0.25)
        pi.clear_queue()
        pi.thread_run = False
        while pi.run_parser():
            pass
        time.sleep(0.01)

        a_offsets, a_gains = crunch_accel(accel_packets)
        m_offsets, m_gains = crunch_mag(accel_packets)
        print("Calculated Accel Offsets: {}".format(a_offsets))
        print("Calculated Accel Gains: {}".format(a_gains))
        print("Calculated Mag Offsets: {}".format(m_offsets))
        print("Calculated Mag Gains: {}".format(m_gains))
        cal_blob = generate_calBlob(m_offsets, m_gains, a_offsets, a_gains)
        print("Blob: {}".format(cal_blob))

        if args.flash:
            print("Flashing this cal to the pensel...")
            retval, response = pi.send_report(0x41, payload=cal_blob)
            if retval != 0:
                print("Failed to write payload! Retval: {} response: {}".format(retval, response))
            retval, response = pi.send_report(0x40)
            if retval != 0:
                print("Failed to read back cal! Retval: {} response: {}".format(retval, response))
            else:
                print("read back cal: Retval: {} response: {}".format(retval, response))

        pi.close()  # cleanup
