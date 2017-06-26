#! /usr/bin/python
import matplotlib.pyplot as plt

import penselreport as pr
import pensel_utils as pu


class LSM303DLHC_Parser(object):
    def __init__(self, port, verbose=0):
        self.verbose = verbose
        self.pensel = pr.Pensel(port, 115200, verbose=verbose)

    def post_plot(self, num_samples=100, plot_mag=True, plot_accel=True):
        """
        """
        # get the packets
        accel_packets = []
        mag_packets = []
        accel_done = False if plot_accel else True
        mag_done = False if plot_mag else True
        while True:
            if plot_accel:
                if len(accel_packets) < num_samples:
                    retval, payload = self.pensel.send_report(0x22, [0])
                    if retval == 0 and len(payload) != 0:
                        frame_num, timestamp, x, y, z = pu.parse_accel_packet(payload)
                        if self.verbose:
                            print("Got Accel packet# {}".format(frame_num))
                        accel_packets.append([x, y, z])
                else:
                    accel_done = True

            if plot_mag:
                if len(mag_packets) < num_samples:
                    retval, payload = self.pensel.send_report(0x23, [0])
                    if retval == 0 and len(payload) != 0:
                        frame_num, timestamp, x, y, z = pu.parse_mag_packet(payload)
                        if self.verbose:
                            print("Got Mag packet# {}".format(frame_num))
                        mag_packets.append([x, y, z])
                else:
                    mag_done = True

            # check if we're done!
            if accel_done and mag_done:
                break

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

    lsm = LSM303DLHC_Parser("/dev/tty.SLAB_USBtoUART", verbose=args.verbose)
    lsm.post_plot(plot_mag=args.mag, plot_accel=args.accel, num_samples=args.samples)
