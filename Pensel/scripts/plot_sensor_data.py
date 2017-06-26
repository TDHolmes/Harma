import os
import math

import matplotlib.pyplot as plt

import penselreport as pr


class LSM303DLHC_Parser(object):
    def __init__(self, port, verbose=0):
        self.accel_packets = []
        self.mag_packets = []
        self.verbose = verbose
        self.pensel = pr.Pensel(port, 115200, verbose=verbose)

    def workloop(self, plot=False):
        """
        """
        X_accel_axis = [i for i in range(len(self.accel_packets))]
        X_mag_axis = [i for i in range(len(self.mag_packets))]

        x_accel = [pkt[0] for pkt in self.accel_packets]
        y_accel = [pkt[1] for pkt in self.accel_packets]
        z_accel = [pkt[2] for pkt in self.accel_packets]

        x_mag = [pkt[0] for pkt in self.mag_packets]
        y_mag = [pkt[1] for pkt in self.mag_packets]
        z_mag = [pkt[2] for pkt in self.mag_packets]

        # Markers: S, 8, >, <, ^, v, o, X, P, d
        fig, ax = plt.subplots()
        line = ax.plot(X_accel_axis, x_accel, color="red", label='Accel: X')
        line = ax.plot(X_accel_axis, y_accel, color="green", label='Accel: Y')
        line = ax.plot(X_accel_axis, z_accel, color="blue", label='Accel: Z')

        line = ax.plot(X_mag_axis, x_mag, color="black", label='Mag: X')
        line = ax.plot(X_mag_axis, y_mag, color="magenta", label='Mag: Y')
        line = ax.plot(X_mag_axis, z_mag, color="cyan", label='Mag: Z')

        plt.ion()
        ax.legend(loc='lower right')
        plt.show()


if __name__ == '__main__':
    import argparse
    parser = argparse.ArgumentParser(description='Parse recorded data from the Pensel.')
    parser.add_argument('file', type=str,
                        help='Filename to playback recorded data.')

    args = parser.parse_args()

    lsm = LSM303DLHC_Parser(args.file)
    lsm.workloop()
