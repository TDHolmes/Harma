import os
import math

# import numpy as np
import matplotlib.pyplot as plt

# Mag 0x14FE -0x0605 0x6A00
# Acc -0x0229 -0x28 0x038B


class LSM303DLHC_Parser(object):
    def __init__(self, filename, verbose=0):
        self.accel_packets = []
        self.mag_packets = []
        self.verbose = verbose
        self.filename = os.path.abspath(os.path.expanduser(filename))

    def parse_data(self):
        if self.filename:
            with open(self.filename, "r") as f:
                for line in f.readlines():
                    linesplit = line.split(" ")

                    if linesplit[0] == "Mag":
                        pkt = (float(linesplit[1]), float(linesplit[2]), float(linesplit[3]))
                        if self.verbose == 2:
                            print("New Mag Packet: {}".format(pkt))
                        self.mag_packets.append(pkt)

                    elif linesplit[0] == "Acc":
                        pkt = (int(linesplit[1], 16), int(linesplit[2], 16), int(linesplit[3], 16))
                        if self.verbose == 2:
                            print("New Accel Packet: {}".format(pkt))
                        self.accel_packets.append(pkt)

                    else:
                        if self.verbose == 1:
                            print('Unrecognized line! "{}"'.format(line))

    def workloop(self, plot=False):
        LEN_BUFF = 10
        self.parse_data()

        X_accel_axis = [i for i in range(len(self.accel_packets))]
        X_mag_axis = [i for i in range(len(self.mag_packets))]

        # x_accel = [pkt[0] for pkt in self.accel_packets]
        # y_accel = [pkt[1] for pkt in self.accel_packets]
        # z_accel = [pkt[2] for pkt in self.accel_packets]
        buff_x = [0 for i in range(LEN_BUFF)]
        buff_y = [0 for i in range(LEN_BUFF)]
        buff_z = [0 for i in range(LEN_BUFF)]
        x_accel = []
        y_accel = []
        z_accel = []
        for ind, pkt in enumerate(self.accel_packets):
            buff_x[ind % 10] = pkt[0]
            buff_y[ind % 10] = pkt[1]
            buff_z[ind % 10] = pkt[2]
            x_accel.append(sum(buff_x) / float(LEN_BUFF))
            y_accel.append(sum(buff_y) / float(LEN_BUFF))
            z_accel.append(sum(buff_z) / float(LEN_BUFF))

        # x_mag = [pkt[0] for pkt in self.mag_packets]
        # y_mag = [pkt[1] for pkt in self.mag_packets]
        # z_mag = [pkt[2] for pkt in self.mag_packets]

        buff_x = [0 for i in range(LEN_BUFF)]
        buff_y = [0 for i in range(LEN_BUFF)]
        buff_z = [0 for i in range(LEN_BUFF)]
        x_mag = []
        y_mag = []
        z_mag = []
        for ind, pkt in enumerate(self.mag_packets):
            buff_x[ind % 10] = pkt[0]
            buff_y[ind % 10] = pkt[1]
            buff_z[ind % 10] = pkt[2]
            x_mag.append(sum(buff_x) / float(LEN_BUFF))
            y_mag.append(sum(buff_y) / float(LEN_BUFF))
            z_mag.append(sum(buff_z) / float(LEN_BUFF))

        print("Accel - X Axis: {}\t\tx_accel: {}".format(len(X_accel_axis), len(x_accel)))
        print("Mag - X Axis: {}\t\tx_accel: {}".format(len(X_mag_axis), len(x_mag)))

        print("Accel Stats (Ave, Min, Max):")
        print("\tX Axis: {: >9.3f} {: >9.3f} {: >9.3f}".format(sum(x_accel) / float(len(x_accel)),
                                                               min(x_accel), max(x_accel)))
        print("\tY Axis: {: >9.3f} {: >9.3f} {: >9.3f}".format(sum(y_accel) / float(len(y_accel)),
                                                               min(y_accel), max(y_accel)))
        print("\tZ Axis: {: >9.3f} {: >9.3f} {: >9.3f}".format(sum(z_accel) / float(len(z_accel)),
                                                               min(z_accel), max(z_accel)))

        print("Mag Stats (Ave, Min, Max):")
        print("\tX Axis: {: >10.3f} {: >10.3f} {: >10.3f}".format(sum(x_mag) / float(len(x_mag)),
                                                                  min(x_mag), max(x_mag)))
        print("\tY Axis: {: >10.3f} {: >10.3f} {: >10.3f}".format(sum(y_mag) / float(len(y_mag)),
                                                                  min(y_mag), max(y_mag)))
        print("\tZ Axis: {: >10.3f} {: >10.3f} {: >10.3f}".format(sum(z_mag) / float(len(z_mag)),
                                                                  min(z_mag), max(z_mag)))
        # Markers: S, 8, >, <, ^, v, o, X, P, d
        fig, ax = plt.subplots()
        line1, = ax.plot(X_accel_axis, x_accel, color="red", label='Accel: X')
        line1, = ax.plot(X_accel_axis, y_accel, color="green", label='Accel: Y')
        line1, = ax.plot(X_accel_axis, z_accel, color="blue", label='Accel: Z')

        # print("Accel magnitudes:")
        # for pkt in self.accel_packets:
        #     print("\t{: >9.3f}".format(math.sqrt(pkt[0]**2 + pkt[1]**2 + pkt[2]**2)))

        line1, = ax.plot(X_mag_axis, x_mag, color="black", label='Mag: X')
        line1, = ax.plot(X_mag_axis, y_mag, color="magenta", label='Mag: Y')
        line1, = ax.plot(X_mag_axis, z_mag, color="cyan", label='Mag: Z')

        # print("Mag magnitudes:")
        # for pkt in self.mag_packets:
        #     print("\t{: >10.3f}".format(math.sqrt(pkt[0]**2 + pkt[1]**2 + pkt[2]**2)))

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
