#! /usr/bin/python
import os
import math
import matplotlib.pyplot as plt

# required import, but unused :( stupid matplotlib
from mpl_toolkits.mplot3d import Axes3D

from penselreport import Pensel
import pensel_utils as pu


plots_supported = {"accel": 0x81, "mag": 0x82}

mag_offsets = [1111.877197265625, 1219.1551513671875, -951.0]
# mag_offsets = [1304.7655029296875, 1176.2099609375, -940.0]


def plot_accel_or_mag(packet, axis, plot_type):
    """
    Plotting accel and mag packets are identical..
    """
    report, retval, payload = packet
    if report == plots_supported[plot_type] and retval == 0:
        axis.clear()
        # Parse it...
        frame_num, timestamp, x, y, z = pu.parse_accel_packet(payload)
        if plot_type == "mag":
            print("applying offsets...")
            x = x - mag_offsets[0]
            y = y - mag_offsets[1]
            z = z - mag_offsets[2]

        print("Plotting packet# {} ({} ms)\t\t({}, {}, {})".format(frame_num, timestamp, x, y, z))
        # plot it
        axis.quiver(0.5, 0.5, 0.5, x, y, z, length=(0.5 / math.sqrt(x**2 + y**2 + z**2)))


def main(port, plot_type):
    with Pensel(port, 115200, args.verbose) as pi:
        plt.ion()
        fig = plt.figure()
        ax = fig.gca(projection='3d')
        # turn on accel & mag streaming
        retval, response = pi.send_report(0x20, payload=[3])
        while True:
            packet = pi.get_packet()
            if packet:
                pi.clear_queue()   # clear it so we don't lag...
                pi._clear_serial = True
                if plot_type == "accel" or plot_type == "mag":
                    plot_accel_or_mag(packet, ax, plot_type)
                ax.set_xlim(0, 1)
                ax.set_ylim(0, 1)
                ax.set_zlim(0, 1)
                ax.set_xlabel('X axis')
                ax.set_ylabel('Y axis')
                ax.set_zlabel('Z axis')
                plt.show()
                plt.pause(0.0001)


if __name__ == '__main__':
    import argparse
    parser = argparse.ArgumentParser(description='Visualize 3D vectors from the pensel.')
    parser.add_argument('plot_type', type=str,
                        help='What type of vector to plot. Can be: {}'.format(plots_supported.keys()))
    parser.add_argument('--baudrate', "-b", dest='baudrate', default=115200,
                        help='Baudrate of the serial data to recieve')
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
    main(port, args.plot_type)
