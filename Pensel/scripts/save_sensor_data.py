#! /usr/bin/env python3
import time
import struct
import matplotlib.pyplot as plt

import pensel_utils as pu


class LSM303DLHC_Parser(object):
    SAMPLES_PER_SECOND = 100  # conservative

    def __init__(self, port, baudrate, verbose=0):
        self.verbose = verbose
        self.port = port
        self.baudrate = baudrate

    def post_plot(self, num_samples=100, save_mag=True, save_accel=True):
        """
        """
        # get the packets
        accel_packets = []
        mag_packets = []
        accel_done = False if save_accel else True
        mag_done = False if save_mag else True

        end_time = (num_samples / self.SAMPLES_PER_SECOND) * 1.2 + time.time()

        with pu.Pensel(self.port, None, self.verbose) as pi:

            # turn on accel & mag streaming
            pi.log("Turning on input streaming...")
            # retval, response = pi.send_report(0x20, payload=[0xc])
            pi.log("Gathering packets...")
            try:
                while time.time() < end_time:
                    packet = pi.get_packet()
                    if packet:
                        report, retval, payload = packet
                        if report == 0x83 and retval == 0 and save_accel:  # accel
                            packed_data = struct.pack("B" * len(payload), *payload)
                            pkt = pi.parse_accel_packet(packed_data)
                            accel_packets.append(pkt)
                            if self.verbose:
                                pi.log("Got Accel packet# {}".format(pkt.frame_num))

                        elif report == 0x84 and retval == 0 and save_mag:  # mag
                            packed_data = struct.pack("B" * len(payload), *payload)
                            pkt = pi.parse_mag_packet(packed_data)
                            mag_packets.append(pkt)
                            if self.verbose:
                                pi.log("Got Mag packet# {}".format(pkt.frame_num))

                    if len(accel_packets) >= num_samples:
                        accel_done = True
                    if len(mag_packets) >= num_samples:
                        mag_done = True

                    # check if we're done!
                    if accel_done and mag_done:
                        pi.log("Done collecting data!")
                        break
            except KeyboardInterrupt:
                print("Keyboard interrupt! Swallowing...")

        if accel_done is False or mag_done is False:
            print("WARNING: Timed out before getting enough packets.")

        # Markers: S, 8, >, <, ^, v, o, X, P, d
        fig, ax = plt.subplots()

        if save_accel:
            with open("accel.csv", "w") as f:
                for pkt in accel_packets:
                    f.write("{},{},{}\n".format(pkt.x, pkt.y, pkt.z))

        if save_mag:
            with open("mag.csv", "w") as f:
                for pkt in mag_packets:
                    f.write("{},{},{}\n".format(pkt.x, pkt.y, pkt.z))


if __name__ == '__main__':
    import argparse
    parser = argparse.ArgumentParser(description='Plot sensor data from the Pensel.')
    # parser.add_argument('file', type=str,
    #                     help='Filename to playback recorded data.')
    parser.add_argument('--mag', action="store_true",
                        help='Determine whether or not to plot mag.')
    parser.add_argument('--accel', action="store_true",
                        help='Determine whether or not to plot accel.')

    parser.add_argument('--verbose', action="store_true",
                        help='Verbose output toggle.')
    parser.add_argument('--samples', type=int, default=100,
                        help='number of samples to aquire before plotting')
    parser.add_argument('--baudrate', type=int, default=None,
                        help='baudrate of coms')

    args = parser.parse_args()

    if not args.accel and not args.mag:
        print("Error: Must specify at least one plot option")
        raise SystemExit(-1)

    lsm = LSM303DLHC_Parser("/dev/tty.SLAB_USBtoUART", args.baudrate, verbose=args.verbose)
    lsm.post_plot(save_mag=args.mag, save_accel=args.accel, num_samples=args.samples)
