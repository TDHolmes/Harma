#! /usr/bin/python
import matplotlib.pyplot as plt
import time

# from penselreport import Pensel
import pensel_utils as pu

mag_offsets = [1111.877197265625, 1219.1551513671875, -951.0]


class LSM303DLHC_Parser(object):
    SAMPLES_PER_SECOND = 100  # conservative

    def __init__(self, port, verbose=0):
        self.verbose = verbose
        self.port = port
        self.baudrate = 115200

    def post_plot(self, num_samples=100, plot_mag=True, plot_accel=True):
        """
        """
        # get the packets
        accel_packets = []
        mag_packets = []
        accel_done = False if plot_accel else True
        mag_done = False if plot_mag else True

        end_time = (num_samples / self.SAMPLES_PER_SECOND) * 1.2 + time.time()

        with pu.Pensel(self.port, self.baudrate, self.verbose) as pi:

            # turn on accel & mag streaming
            pi.log("Turning on input streaming...")
            retval, response = pi.send_report(0x20, payload=[0xc])
            pi.log("Gathering packets...")
            try:
                while time.time() < end_time:
                    packet = pi.get_packet()
                    if packet:
                        report, retval, payload = packet
                        if report == 0x83 and retval == 0 and plot_accel:  # accel
                            frame_num, timestamp, x, y, z = pu.parse_accel_packet(payload)
                            accel_packets.append([x, y, z])
                            if self.verbose:
                                pi.log("Got Accel packet# {}".format(frame_num))
                        elif report == 0x84 and retval == 0 and plot_mag:  # mag
                            frame_num, timestamp, x, y, z = pu.parse_mag_packet(payload)
                            # mag_packets.append([x - mag_offsets[0], y - mag_offsets[1], z - mag_offsets[2]])
                            mag_packets.append([x, y, z])
                            if self.verbose:
                                pi.log("Got Mag packet# {}".format(frame_num))

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

        if plot_accel:
            X_accel_axis = [i for i in range(len(accel_packets))]
            x_accel = [pkt[0] for pkt in accel_packets]
            y_accel = [pkt[1] for pkt in accel_packets]
            z_accel = [pkt[2] for pkt in accel_packets]
            ax.plot(X_accel_axis, x_accel, color="red", label='Accel: X')
            ax.plot(X_accel_axis, y_accel, color="green", label='Accel: Y')
            ax.plot(X_accel_axis, z_accel, color="blue", label='Accel: Z')

        if plot_mag:
            X_mag_axis = [i for i in range(len(mag_packets))]
            x_mag = [pkt[0] for pkt in mag_packets]
            y_mag = [pkt[1] for pkt in mag_packets]
            z_mag = [pkt[2] for pkt in mag_packets]

            ax.plot(X_mag_axis, x_mag, color="black", label='Mag: X')
            ax.plot(X_mag_axis, y_mag, color="magenta", label='Mag: Y')
            ax.plot(X_mag_axis, z_mag, color="cyan", label='Mag: Z')

        # plt.ion()
        ax.legend(loc='lower right')
        plt.show()


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

    args = parser.parse_args()

    if not args.accel and not args.mag:
        print("Error: Must specify at least one of mag or accel")
        raise SystemExit(-1)

    lsm = LSM303DLHC_Parser("/dev/tty.SLAB_USBtoUART", verbose=args.verbose)
    lsm.post_plot(plot_mag=args.mag, plot_accel=args.accel, num_samples=args.samples)
